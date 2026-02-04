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
#include "GameEngine/Resources/ShaderBuffers/PerObjectConstants.h"
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
    , measurementValue_(context.measurmentHandler_.AddNewMeasurment("TreeRenderer", "0"))
{
}

void TreeRenderer::init()
{
    if (not paramBufferId_)
    {
        paramBufferId_ = context_.graphicsApi_.CreateShaderBuffer(4, sizeof(TreeParamBuffer), GraphicsApi::DrawFlag::Dynamic);
        perObjectConstantsBufferId = context_.graphicsApi_.CreateShaderBuffer(
            PER_OBJECT_CONSTANTS_BIND_LOCATION, sizeof(PerObjectConstants), GraphicsApi::DrawFlag::Dynamic);

        if (perObjectConstantsBufferId)
        {
            PerObjectConstants perObjectConstants;
            perObjectConstants.UseBoneTransform = 0.f;
            perObjectConstants.textureOffset    = vec2{0.f};
            context_.graphicsApi_.UpdateShaderBuffer(*perObjectConstantsBufferId, &perObjectConstants);
        }
    }
}

void TreeRenderer::render()
{
    int rendererModels = 0;

    if (subscribes_.empty())
        return;

    if (perObjectConstantsBufferId)
    {
        context_.graphicsApi_.BindShaderBuffer(*perObjectConstantsBufferId);
    }

    for (auto& sub : subscribes_)
    {
        for (auto* treeRendererComponent : sub.treeRendererComponent_)
        {
            auto isVisible = context_.frustrum_.intersection(treeRendererComponent->GetWorldBoundingBox());
            if (isVisible)
            {
                auto perUpdate = treeRendererComponent->GetPerObjectUpdateId();
                if (perUpdate)
                {
                    context_.graphicsApi_.BindShaderBuffer(*perUpdate);
                }

                auto distanceToCamera = getDistanceToCamera(*treeRendererComponent);
                UpdateTreePramBuffer(*treeRendererComponent, distanceToCamera);
                RenderTree(*treeRendererComponent, distanceToCamera);
                ++rendererModels;
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
void TreeRenderer::RenderModel(const Model& model) const
{
    for (const auto& mesh : model.GetMeshes())
    {
        if (mesh.GetGraphicsObjectId())
        {
            const auto& buffer = mesh.GetMaterialShaderBufferId();
            context_.graphicsApi_.BindShaderBuffer(*buffer);
            RenderMesh(mesh);
        }
    }
}
void TreeRenderer::RenderMesh(const Mesh& mesh) const
{
    BindMaterial(mesh.GetMaterial());
    context_.graphicsApi_.RenderMesh(*mesh.GetGraphicsObjectId());
}
void TreeRenderer::UpdateTreePramBuffer(Components::TreeRendererComponent& treeRendererComponent, float distanceToCamera)
{
    if (paramBufferId_)
    {
        TreeParamBuffer buffer;
        treeRendererComponent.windTime += 0.1f * context_.time_.deltaTime;
        buffer.time   = treeRendererComponent.windTime;
        buffer.wind   = vec4(glm::normalize(glm::vec3(0.6f, 0.0f, 0.8f)),
                           context_.scene_ ? context_.scene_->getWindParams().windStrength : 0.4f);
        buffer.fprams = vec4{0, 0, 0, CalculateFadeFactor(distanceToCamera)};
        buffer.atlasParams =
            vec4i{treeRendererComponent.leafTextureAtlasSize.x, treeRendererComponent.leafTextureAtlasSize.y, 0, 0};

        context_.graphicsApi_.UpdateShaderBuffer(*paramBufferId_, &buffer);
        context_.graphicsApi_.BindShaderBuffer(*paramBufferId_);
    }
}
float TreeRenderer::getDistanceToCamera(Components::IComponent& component) const
{
    const auto& position       = component.getParentGameObject().GetWorldTransform().GetPosition();
    const auto& cameraPosition = context_.camera_->GetPosition();

    return glm::distance(position, cameraPosition);
}
float TreeRenderer::CalculateFadeFactor(float distanceToCamera)
{
    return glm::clamp((distanceToCamera - startFade) / (endFade - startFade), 0.0f, 1.0f);
}
void TreeRenderer::cleanUp()
{
    if (paramBufferId_)
    {
        context_.graphicsApi_.DeleteShaderBuffer(*paramBufferId_);
        paramBufferId_.reset();
    }

    if (perObjectConstantsBufferId)
    {
        context_.graphicsApi_.DeleteShaderBuffer(*perObjectConstantsBufferId);
        perObjectConstantsBufferId.reset();
    }
}
}  // namespace GameEngine
