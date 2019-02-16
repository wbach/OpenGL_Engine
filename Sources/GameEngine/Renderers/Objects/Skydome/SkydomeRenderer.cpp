#include "SkydomeRenderer.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Components/Renderer/Skydome/SkydomeComponent.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
SkydomRenderer::SkydomRenderer(RendererContext& context)
    : context_(context)
{
    shader_ = context_.shaderFactory_.create(GraphicsApi::Shaders::Skydome);
    __RegisterRenderFunction__(RendererFunctionType::CONFIGURE, SkydomRenderer::Render);
}
void SkydomRenderer::Init()
{
    shader_->Init();
}
void SkydomRenderer::Subscribe(GameObject* gameObject)
{
    auto component = gameObject->GetComponent<Components::SkydomeComponent>();

    if (component == nullptr)
        return;

    subscribes_.push_back({component->GetModel()});
}
void SkydomRenderer::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    shader_->Init();
}
void SkydomRenderer::Render(const Scene& scene, const Time&)
{
    if (subscribes_.empty())
        return;

    shader_->Start();
}
}  // namespace GameEngine
