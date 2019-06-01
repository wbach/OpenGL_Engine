#include "WaterRenderer.h"
#include "GameEngine/Components/Renderer/Water/WaterRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
WaterRenderer::WaterRenderer(RendererContext& context)
    : context_(context)
{
    shader_ = context.shaderFactory_.create(GraphicsApi::Shaders::Water);
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, WaterRenderer::Render);
}
void WaterRenderer::Init()
{
    shader_->Init();

    if (not perObjectUpdateId_)
    {
        perObjectUpdateId_ =
            context_.graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));
    }
}
void WaterRenderer::Render(const Scene& scene, const Time&)
{
    shader_->Start();

    for (auto& subscriber : subscribers_)
    {
        context_.graphicsApi_.UpdateShaderBuffer(*perObjectUpdateId_, &subscriber.second.perObjectUpdate_);
        context_.graphicsApi_.BindShaderBuffer(*perObjectUpdateId_);
        context_.graphicsApi_.RenderQuad();
    }
}
PerObjectUpdate WaterRenderer::CalculateTransformMatrix(const vec3& position, const vec3& scale) const
{
    return {Utils::CreateTransformationMatrix(position, vec3(90, 0, 0), scale)};
}
void WaterRenderer::Subscribe(GameObject* gameObject)
{
    auto waterComponent = gameObject->GetComponent<Components::WaterRendererComponent>();

    if (not waterComponent)
    {
        return;
    }

    subscribers_.insert(
        {gameObject->GetId(),
         {CalculateTransformMatrix(waterComponent->GetPosition(), waterComponent->GetScale()), waterComponent}});
}
void WaterRenderer::UnSubscribe(GameObject* gameObject)
{
    subscribers_.erase(gameObject->GetId());
}
void WaterRenderer::UnSubscribeAll()
{
    subscribers_.clear();
}
void WaterRenderer::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    shader_->Init();
}
}  // namespace GameEngine
