#include "GuiRenderer.h"
#include "Text/GuiTextRenderer.h"
#include "Texutre/GuiTextureRenderer.hpp"

namespace GameEngine
{
GUIRenderer::GUIRenderer(GraphicsApi::IGraphicsApi& graphicsApi, IShaderFactory& shaderFactory)
    : graphicsApi_(graphicsApi)
{
    renderers_.emplace_back(new GuiTextRenderer(graphicsApi_, shaderFactory));
    renderers_.emplace_back(new Renderer::Gui::GuiTextureRenderer(graphicsApi_, shaderFactory));
}
GUIRenderer::~GUIRenderer()
{
}
void GUIRenderer::Init()
{
    for (const auto& renderer : renderers_)
    {
        renderer->Init();
    }
}

void GUIRenderer::Render(const Scene&, const Time&)
{
    for (const auto& renderer : renderers_)
    {
        renderer->Render();
    }
}

void GUIRenderer::Add(const std::string& name, std::unique_ptr<GuiElement> element)
{
    for (const auto& renderer : renderers_)
    {
        renderer->Subscribe(element.get());
    }

    elementsMap_.insert({ name , element.get()});
    elements_.push_back(std::move(element));
}

void GUIRenderer::UnSubscribeAll()
{
    for (auto& renderer : renderers_)
        renderer->UnSubscribeAll();

    elements_.clear();
    elementsMap_.clear();
}
void GUIRenderer::ReloadShaders()
{
    for (auto& renderer : renderers_)
        renderer->ReloadShaders();
}
}  // namespace GameEngine
