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

void GUIRenderer::UnSubscribeAll()
{
    for (auto& renderer : renderers_)
        renderer->UnSubscribeAll();
}
void GUIRenderer::ReloadShaders()
{
    for (auto& renderer : renderers_)
        renderer->ReloadShaders();
}
void GUIRenderer::Subscribe(GuiElement& elemnt)
{
    for (auto& renderer : renderers_)
        renderer->Subscribe(&elemnt);
}
}  // namespace GameEngine
