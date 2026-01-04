#include "TreeLeafClusterRenderer.h"

#include <Common/Transform.h>
#include <Utils/MeasurementHandler.h>

#include <algorithm>

#include "GameEngine/Camera/Frustrum.h"
#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Projection/IProjection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/PerMeshObject.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "GameEngine/Resources/ITextureLoader.h"
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

void TreeLeafClusterRenderer::render(const Model& model, ResultCallback resultCallback)
{
    leafsClusterShader.Init();

    const vec2ui renderSize{512, 512};
    GraphicsApi::FrameBuffer::Attachment depthAttachment(renderSize, GraphicsApi::FrameBuffer::Type::Depth,
                                                         GraphicsApi::FrameBuffer::Format::Depth);

    depthAttachment.wrapMode = GraphicsApi::FrameBuffer::WrapMode::ClampToEdge;
    depthAttachment.filter   = GraphicsApi::FrameBuffer::Filter::Linear;

    GraphicsApi::FrameBuffer::Attachment colorAttachment(renderSize, GraphicsApi::FrameBuffer::Type::Color0,
                                                         GraphicsApi::FrameBuffer::Format::Rgba8);

    colorAttachment.filter = GraphicsApi::FrameBuffer::Filter::Linear;

    auto frameBuffer = &graphicsApi.CreateFrameBuffer({depthAttachment, colorAttachment});

    if (not frameBuffer)
    {
        LOG_ERROR << "Unexpected error";
        return;
    }

    frameBuffer->Bind();

    auto bufferId =
        graphicsApi.CreateShaderBuffer(PER_FRAME_BIND_LOCATION, sizeof(PerFrameBuffer), GraphicsApi::DrawFlag::Static);
    if (not bufferId)
    {
        LOG_ERROR << "Unexpected error";
        return;
    }

    // --- parametry clustra ---
    const float R         = 1.0f;  // promień wzorcowego clustra
    const float margin    = 1.2f;
    const float orthoSize = R * margin;

    // --- kamera w local space ---
    vec3 cameraPosition = vec3(0, 0, -2.0f * R);
    auto viewMatrix     = glm::lookAt(cameraPosition,  // eye
                                      vec3(0, 0, 0),   // center
                                      vec3(0, 1, 0)    // up
        );

    auto projectionMatrix = glm::ortho(-orthoSize, orthoSize,  // left, right
                                       -orthoSize, orthoSize,  // bottom, top
                                       -orthoSize, orthoSize   // near, far
    );

    auto projectionViewMatrix = projectionMatrix * viewMatrix;

    PerFrameBuffer perFrameBuffer;
    perFrameBuffer.ProjectionViewMatrix = graphicsApi.PrepareMatrixToLoad(projectionViewMatrix);
    perFrameBuffer.cameraPosition       = cameraPosition;
    perFrameBuffer.clipPlane            = vec4(0.f, 1.f, 0.f, 0.f);  // opcjonalnie
    perFrameBuffer.projection           = vec4(0);                   // camera.GetProjection().GetBufferParams();

    graphicsApi.UpdateShaderBuffer(*bufferId, &perFrameBuffer);
    graphicsApi.BindShaderBuffer(*bufferId);

    GenerateClusterAtlasTexture(*frameBuffer, model);
    frameBuffer->UnBind();

    resultCallback(resultTextures);
}
void TreeLeafClusterRenderer::GenerateClusterAtlasTexture(GraphicsApi::IFrameBuffer& frameBuffer, const Model& model)
{
    for (auto& mesh : model.GetMeshes())
    {
        const vec2ui renderSize{512, 512};
        graphicsApi.SetViewPort(0, 0, renderSize.x, renderSize.y);

        if (mesh.GetGraphicsObjectId() && !mesh.GetMaterial().baseColorTexture)
        {
            leafsClusterShader.Start();
            const auto& buffer = mesh.GetMaterialShaderBufferId();
            graphicsApi.BindShaderBuffer(*buffer);

            graphicsApi.RenderPoints(*mesh.GetGraphicsObjectId());

            auto material = mesh.GetMaterial();
            if (auto maybeNewNexture = cloneAttachmentTexture(frameBuffer))
            {
                resultTextures.push_back(maybeNewNexture);
            }
        }
    }
}
GeneralTexture* TreeLeafClusterRenderer::cloneAttachmentTexture(const GraphicsApi::IFrameBuffer& frameBuffer)
{
    auto image = frameBuffer.GetImage(GraphicsApi::FrameBuffer::Type::Color0);
    if (not image)
    {
        LOG_WARN << "GetImage error";
        return {};
    }

    //auto newTextureId = graphicsApi.CreateTexture(*image, GraphicsApi::TextureFilter::LINEAR, GraphicsApi::TextureMipmap::LINEAR);
    TextureParameters textureParameters;
    auto newTexture =  resourceManager.GetTextureLoader().CreateTexture("", textureParameters, std::move(*image));
    return newTexture;
}
}  // namespace GameEngine
