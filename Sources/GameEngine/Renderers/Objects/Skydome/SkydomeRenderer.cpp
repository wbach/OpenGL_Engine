#include "SkydomeRenderer.h"

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/Renderer/Skydome/SkydomeComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
SkydomRenderer::SkydomRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Skydome)
{
}
void SkydomRenderer::init()
{
    shader_.Init();

    if (not perObjectUpdateId_)
    {
        perObjectUpdateId_ =
            context_.graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));
        UpdateBuffer(vec3(0));
    }
}
void SkydomRenderer::subscribe(GameObject& gameObject)
{
    auto component = gameObject.GetComponent<Components::SkydomeComponent>();

    if (component == nullptr)
        return;

    subscriber_.model_ = component->GetModel();
}
void SkydomRenderer::reloadShaders()
{
    shader_.Reload();
}
void SkydomRenderer::unSubscribe(GameObject& gameObject)
{
    auto component = gameObject.GetComponent<Components::SkydomeComponent>();

    if (component)
        subscriber_.model_ = nullptr;
}
void SkydomRenderer::unSubscribeAll()
{
    subscriber_.model_ = nullptr;
}
void SkydomRenderer::render()
{
    if (not subscriber_.model_)
        return;

    shader_.Start();
    auto position = context_.scene_ ? context_.scene_->GetCamera().GetPosition() : vec3(0);
    UpdateBuffer(position);
    context_.graphicsApi_.BindShaderBuffer(*perObjectUpdateId_);

    for (const auto& mesh : subscriber_.model_->GetMeshes())
    {
        if (mesh.GetGraphicsObjectId())
            context_.graphicsApi_.RenderMesh(*mesh.GetGraphicsObjectId());
    }

    shader_.Stop();
}
void SkydomRenderer::UpdateBuffer(const vec3& cameraPosition)
{
    vec3 position = cameraPosition;
    position.y = 0;

    perObjectUpdate_.TransformationMatrix =
        Utils::CreateTransformationMatrix(position, DegreesVec3(0), vec3(context_.projection_.GetViewDistance()));
    context_.graphicsApi_.UpdateShaderBuffer(*perObjectUpdateId_, &perObjectUpdate_);
}
}  // namespace GameEngine
