#include "TreeLeafClusterRenderer.h"

#include <Common/Transform.h>
#include <Utils/MeasurementHandler.h>

#include <algorithm>
#include <cstddef>

#include "GameEngine/Camera/Frustrum.h"
#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/Renderer/Trees/Leaf.h"
#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Projection/IProjection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/PerMeshObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "GraphicsApi/IFrameBuffer.h"
#include "Image/Image.h"
#include "Logger/Log.h"
#include "Types.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/geometric.hpp"

namespace GameEngine
{
namespace
{
struct TreeParamBuffer
{
    AlignWrapper<vec4> fprams;
    AlignWrapper<vec4i> atlasParams;
};
}  // namespace

TreeLeafClusterRenderer::TreeLeafClusterRenderer(GraphicsApi::IGraphicsApi& graphicsApi, IResourceManager& resourceManager)
    : graphicsApi(graphicsApi)
    , resourceManager(resourceManager)
    , leafsClusterShader(graphicsApi, GraphicsApi::ShaderProgramType::TreeLeafsCluster)
{
}
void TreeLeafClusterRenderer::render(const TreeClusters& clusters, const std::vector<Leaf>& allLeaves,
                                     const Material& leafMaterial, ResultCallback resultCallback)
{
    PerObjectUpdate perObjectUpdate;

    transformBuferId =
        graphicsApi.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(perObjectUpdate), GraphicsApi::DrawFlag::Dynamic);

    if (not transformBuferId)
    {
        LOG_ERROR << "Create shaderbuffer error";
        resultCallback(std::nullopt);
        return;
    }

    {
        auto leafsSSBOData = PrepareSSBOData(allLeaves);
        LOG_DEBUG << "Create shaderStorageVectorBufferObject";
        auto totalSize = static_cast<uint32>(leafsSSBOData.size() * sizeof(LeafSSBO));
        leafsSsbo = graphicsApi.CreateShaderStorageBuffer(PER_INSTANCES_BIND_LOCATION, totalSize, GraphicsApi::DrawFlag::Dynamic);

        if (not leafsSsbo)
        {
            LOG_ERROR << "Leafs ssbo for cluster creation error.";
            resultCallback(std::nullopt);
            return;
        }
        graphicsApi.UpdateShaderStorageBuffer(*leafsSsbo, leafsSSBOData.data(), totalSize);
        graphicsApi.BindShaderBuffer(*leafsSsbo);
    }

    {
        leafIndicesBufferId =
            graphicsApi.CreateShaderStorageBuffer(PER_INSTANCES_BIND_LOCATION, 0, GraphicsApi::DrawFlag::Dynamic);
    }

    const vec2ui renderSize{256, 256};
    GraphicsApi::FrameBuffer::Attachment depthAttachment(renderSize, GraphicsApi::FrameBuffer::Type::Depth,
                                                         GraphicsApi::FrameBuffer::Format::Depth);

    depthAttachment.wrapMode = GraphicsApi::FrameBuffer::WrapMode::ClampToEdge;
    depthAttachment.filter   = GraphicsApi::FrameBuffer::Filter::Linear;

    auto frameBuffer = &graphicsApi.CreateFrameBuffer({depthAttachment});

    if (not frameBuffer)
    {
        LOG_ERROR << "Unexpected error";
        resultCallback(std::nullopt);
        return;
    }

    size_t totalLayers = clusters.clusters.size() * 2;
    std::vector<Utils::Image> images;
    images.resize(totalLayers);
    for (auto& image : images)
    {
        image.width     = renderSize.x;
        image.height    = renderSize.y;
        image.channels_ = 4;
        image.allocateImage<uint8>();
    }

    auto textureArrayId = graphicsApi.CreateTexture(images, GraphicsApi::TextureFilter::LINEAR, GraphicsApi::TextureMipmap::NONE);

    if (not textureArrayId)
    {
        LOG_ERROR << "Create texture array error.";
        resultCallback(std::nullopt);
        return;
    }

    auto normalTextureArrayId =
        graphicsApi.CreateTexture(images, GraphicsApi::TextureFilter::LINEAR, GraphicsApi::TextureMipmap::NONE);

    if (not normalTextureArrayId)
    {
        LOG_ERROR << "Create texture array error.";
        resultCallback(std::nullopt);
        return;
    }

    frameBuffer->Bind();

    RenderClusters(*textureArrayId, *normalTextureArrayId, *frameBuffer, clusters, allLeaves, leafMaterial, renderSize);

    graphicsApi.DeleteShaderBuffer(*transformBuferId);
    graphicsApi.DeleteShaderBuffer(*leafsSsbo);
    graphicsApi.DeleteFrameBuffer(*frameBuffer);

    graphicsApi.GenerateMipmaps(*textureArrayId);
    graphicsApi.GenerateMipmaps(*normalTextureArrayId);

    resultCallback(Result{.baseColorTextureArray = textureArrayId, .normalTextureArray = normalTextureArrayId});
}
void TreeLeafClusterRenderer::RenderClusters(IdType textureArrayId, IdType normalTextureArrayId, GraphicsApi::IFrameBuffer& fb,
                                             const TreeClusters& treeData, const std::vector<Leaf>& allLeaves,
                                             const Material& leafMaterial, const vec2ui& renderSize)
{
    leafsClusterShader.Start();
    graphicsApi.SetViewPort(0, 0, renderSize.x, renderSize.y);

    for (size_t i = 0; i < treeData.clusters.size(); ++i)
    {
        const auto& cluster = treeData.clusters[i];

        vec3 center    = (cluster.minBound + cluster.maxBound) * 0.5f;
        float halfSize = (cluster.maxBound.x - cluster.minBound.x) * 0.5f;

        mat4 projection = glm::ortho(-halfSize, halfSize, -halfSize, halfSize, 0.0f, halfSize * 2.0f);

        // --- RENDER WIDOKU 0 (FRONT - Oś Z) ---
        fb.BindTextureLayer(textureArrayId, GraphicsApi::FrameBuffer::Type::Color0, i * 2);
        fb.BindTextureLayer(normalTextureArrayId, GraphicsApi::FrameBuffer::Type::Color1, i * 2);
        fb.Clear();

        mat4 viewFront = lookAt(center + vec3(0, 0, halfSize), center, vec3(0, 1, 0));
        DrawClusterLeaves(cluster, allLeaves, leafMaterial, projection * viewFront);

        // --- RENDER WIDOKU 1 (SIDE - Oś X) ---
        fb.BindTextureLayer(textureArrayId, GraphicsApi::FrameBuffer::Type::Color0, i * 2 + 1);
        fb.BindTextureLayer(normalTextureArrayId, GraphicsApi::FrameBuffer::Type::Color1, i * 2 + 1);
        fb.Clear();

        mat4 viewSide = lookAt(center + vec3(halfSize, 0, 0), center, vec3(0, 1, 0));
        DrawClusterLeaves(cluster, allLeaves, leafMaterial, projection * viewSide);
    }
}

void TreeLeafClusterRenderer::DrawClusterLeaves(const Cluster& cluster, const std::vector<Leaf>& allLeaves,
                                                const Material& leafMaterial, const mat4& mvp)
{
    if (not transformBuferId)
    {
        return;
    }

    PerObjectUpdate perObjectUpdate;
    perObjectUpdate.TransformationMatrix = graphicsApi.PrepareMatrixToLoad(mvp);
    graphicsApi.UpdateShaderBuffer(*transformBuferId, &perObjectUpdate);
    graphicsApi.BindShaderBuffer(*transformBuferId);

    graphicsApi.UpdateShaderStorageBuffer(*leafIndicesBufferId, cluster.leafIndices.data(),
                                          cluster.leafIndices.size() * sizeof(uint32));
    graphicsApi.BindShaderBuffer(*leafIndicesBufferId);

    BindMaterial(leafMaterial);
    graphicsApi.RenderProcedural(cluster.leafIndices.size() * 6);
}
void TreeLeafClusterRenderer::BindMaterial(const Material& material) const
{
    if (material.flags & MAT_DOUBLE_SIDED || (material.flags & MAT_FOLIAGE))
    {
        graphicsApi.DisableCulling();
    }
    else
    {
        graphicsApi.EnableCulling();
    }

    const auto& config = EngineConf.renderer.textures;

    BindMaterialTexture(0, material.baseColorTexture, config.useDiffuse);
    BindMaterialTexture(1, material.normalTexture, config.useNormal);
    BindMaterialTexture(2, material.roughnessTexture, config.useRoughness);
    BindMaterialTexture(3, material.metallicTexture, config.useMetallic);
    BindMaterialTexture(4, material.ambientOcclusionTexture, config.useAmientOcclusion);
    BindMaterialTexture(5, material.opacityTexture, config.useOpacity);
    BindMaterialTexture(6, material.displacementTexture, config.useDisplacement);
}

void TreeLeafClusterRenderer::UnBindMaterial(const Material& material) const
{
    graphicsApi.EnableCulling();
}
void TreeLeafClusterRenderer::BindMaterialTexture(uint32 location, GeneralTexture* texture, bool enabled) const
{
    if (enabled and texture and texture->GetGraphicsObjectId())
    {
        graphicsApi.ActiveTexture(location, *texture->GetGraphicsObjectId());
    }
}
}  // namespace GameEngine
