#include "TreeRenderer.h"

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
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/PerMeshObject.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "GameEngine/Scene/Scene.hpp"
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
    AlignWrapper<vec4> wind;
    AlignWrapper<vec4> fprams;
    AlignWrapper<vec4i> atlasParams;
    AlignWrapper<float> time;
};
}  // namespace
TreeRenderer::TreeRenderer(RendererContext& context)
    : context_(context)
    , leafsShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::TreeLeafs)
    , trunkShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Entity)
    , measurementValue_(context.measurmentHandler_.AddNewMeasurment("TreeRenderer", "0"))
{
}

TreeRenderer::TreeRenderer(RendererContext& context, GraphicsApi::ShaderProgramType leafsShader,
                           GraphicsApi::ShaderProgramType trunkShader)
    : context_(context)
    , leafsShader_(context.graphicsApi_, leafsShader)
    , trunkShader_(context.graphicsApi_, trunkShader)
    , measurementValue_(context.measurmentHandler_.AddNewMeasurment("TreeRenderer", "0"))
    , simpleRendering{true}
{
}

void TreeRenderer::init()
{
    leafsShader_.Init();
    trunkShader_.Init();

    if (not paramBufferId_)
    {
        paramBufferId_ = context_.graphicsApi_.CreateShaderBuffer(4, sizeof(TreeParamBuffer), GraphicsApi::DrawFlag::Dynamic);
    }
}

void TreeRenderer::render()
{
    int rendererModels = 0;

    if (subscribes_.empty())
        return;

    trunkShader_.Start();

    for (auto& sub : subscribes_)
    {
        for (auto* treeRendererComponent_ : sub.treeRendererComponent_)
        {
            auto perUpdate = treeRendererComponent_->GetPerObjectUpdateId();
            if (perUpdate)
            {
                context_.graphicsApi_.BindShaderBuffer(*perUpdate);
            }

            if (paramBufferId_)
            {
                TreeParamBuffer buffer;
                treeRendererComponent_->windTime += 0.1f * context_.time_.deltaTime;
                buffer.time   = treeRendererComponent_->windTime;
                buffer.wind   = vec4(glm::normalize(glm::vec3(0.6f, 0.0f, 0.8f)),
                                   context_.scene_ ? context_.scene_->getWindParams().windStrength : 0.4f);
                buffer.fprams = vec4{treeRendererComponent_->leafScale, 0, 0, 0};
                buffer.atlasParams =
                    vec4i{treeRendererComponent_->leafTextureAtlasSize, treeRendererComponent_->leafTextureIndex, 0, 0};

                context_.graphicsApi_.UpdateShaderBuffer(*paramBufferId_, &buffer);
                context_.graphicsApi_.BindShaderBuffer(*paramBufferId_);
            }

            if (treeRendererComponent_->GetInstancesSize() > 0)
            {
                rendererModels += RenderInstancedTree(*treeRendererComponent_);
            }
            else
            {
                rendererModels += RenderSingleTree(*treeRendererComponent_);
            }
        }
    }

    measurementValue_ << rendererModels;
}
void TreeRenderer::subscribe(GameObject& gameObject)
{
    auto component = gameObject.GetComponent<Components::TreeRendererComponent>();

    if (not component)
        return;

    auto iter = std::find_if(subscribes_.begin(), subscribes_.end(),
                             [&gameObject](const TreeSubscriber& sub) { return gameObject.GetId() == sub.gameObject_->GetId(); });

    if (iter == subscribes_.end())
    {
        subscribes_.push_back({&gameObject, {component}});
    }
    else
    {
        iter->treeRendererComponent_.push_back(component);
    }
}
void TreeRenderer::unSubscribe(GameObject& gameObject)
{
    auto sub = std::find_if(subscribes_.begin(), subscribes_.end(),
                            [&gameObject](const TreeSubscriber& sub) { return gameObject.GetId() == sub.gameObject_->GetId(); });

    if (sub != subscribes_.end())
    {
        subscribes_.erase(sub);
    }
}
void TreeRenderer::unSubscribeAll()
{
    subscribes_.clear();
}
void TreeRenderer::reloadShaders()
{
    leafsShader_.Reload();
    trunkShader_.Reload();
}
void TreeRenderer::BindMaterial(const Material& material) const
{
    if (material.flags & MAT_DOUBLE_SIDED || (material.flags & MAT_FOLIAGE))
    {
        context_.graphicsApi_.DisableCulling();
    }
    else
    {
        context_.graphicsApi_.EnableCulling();
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

void TreeRenderer::UnBindMaterial(const Material& material) const
{
    context_.graphicsApi_.EnableCulling();
}
void TreeRenderer::BindMaterialTexture(uint32 location, GeneralTexture* texture, bool enabled) const
{
    if (enabled and texture and texture->GetGraphicsObjectId())
    {
        context_.graphicsApi_.ActiveTexture(location, *texture->GetGraphicsObjectId());
    }
}
int TreeRenderer::RenderSingleTree(const Components::TreeRendererComponent& treeRendererComponent)
{
    int renderedCount = 0;
    auto isVisible    = context_.frustrum_.intersection(treeRendererComponent.GetWorldBoundingBox());
    if (not isVisible)
    {
        return 0;
    }

    const auto& position       = treeRendererComponent.getParentGameObject().GetWorldTransform().GetPosition();
    const auto& cameraPosition = context_.camera_->GetPosition();
    auto disnaceToCamera       = glm::distance(position, cameraPosition);
    auto lvl                   = LevelOfDetail::L1;
    if (disnaceToCamera > EngineConf.renderer.lodDistance0 or simpleRendering)
    {
        lvl = LevelOfDetail::L2;
    }

    if (auto model = treeRendererComponent.GetTrunkModel().Get(lvl))
    {
        renderedCount += RenderModel(*model);
    }

    RenderLeafs(treeRendererComponent, lvl);

    return renderedCount;
}
int TreeRenderer::RenderInstancedTree(const Components::TreeRendererComponent& treeRendererComponent)
{
    int renderedCount = 0;
    auto perInstance  = treeRendererComponent.GetPerInstancesBufferId();
    if (perInstance)
    {
        context_.graphicsApi_.BindShaderBuffer(*perInstance);
    }

    if (auto model = treeRendererComponent.GetTrunkModel().Get(LevelOfDetail::L1))
    {
        renderedCount += RenderModel(*model, treeRendererComponent.GetInstancesSize());
    }

    return renderedCount;
}

int TreeRenderer::RenderModel(const Model& model) const
{
    int renderedCount = 0;

    trunkShader_.Start();
    for (const auto& mesh : model.GetMeshes())
    {
        if (mesh.GetGraphicsObjectId())
        {
            const auto& buffer = mesh.GetMaterialShaderBufferId();
            context_.graphicsApi_.BindShaderBuffer(*buffer);
            renderedCount += RenderMesh(mesh);
        }
    }
    trunkShader_.Stop();

    return renderedCount;
}
int TreeRenderer::RenderModel(const Model& model, uint32 count) const
{
    int renderedCount = 0;

    for (const auto& mesh : model.GetMeshes())
    {
        if (mesh.GetGraphicsObjectId())
        {
            const auto& buffer = mesh.GetMaterialShaderBufferId();
            context_.graphicsApi_.BindShaderBuffer(*buffer);
            renderedCount += RenderMesh(mesh, count);
        }
    }
    return renderedCount;
}
int TreeRenderer::RenderMesh(const Mesh& mesh) const
{
    BindMaterial(mesh.GetMaterial());
    context_.graphicsApi_.RenderMesh(*mesh.GetGraphicsObjectId());
    return 1;
}
int TreeRenderer::RenderMesh(const Mesh& mesh, uint32 count) const
{
    BindMaterial(mesh.GetMaterial());
    context_.graphicsApi_.RenderMeshInstanced(*mesh.GetGraphicsObjectId(), count);
    return count;
}
void TreeRenderer::RenderLeafs(const Components::TreeRendererComponent& treeRendererComponent, LevelOfDetail lvl) const
{
    const auto leafModel = treeRendererComponent.GetLeafModel().Get(lvl);

    if (not leafModel)
        return;

    leafsShader_.Start();
    for (const auto& mesh : leafModel->GetMeshes())
    {
        if (not mesh.GetGraphicsObjectId())
        {
            continue;
        }
        BindMaterial(mesh.GetMaterial());
        context_.graphicsApi_.RenderPoints(*mesh.GetGraphicsObjectId());
    }
    leafsShader_.Stop();
}
}  // namespace GameEngine
