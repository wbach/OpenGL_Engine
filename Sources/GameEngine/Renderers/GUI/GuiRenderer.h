#pragma once
#include <memory>
#include <vector>
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererFunctionType.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "GuiElement.h"
#include "IGuiRenderer.h"
#include "Logger/Log.h"

namespace GameEngine
{
struct Time;
class GUIRenderer
{
public:
    GUIRenderer(GraphicsApi::IGraphicsApi& graphicsApi, IShaderFactory& shaderFactory);
    ~GUIRenderer();
    void Init();
    void UnSubscribeAll();
    void UnSubscribe(uint32);
    void UnSubscribe(GuiElement&);
    void ReloadShaders();
    void Subscribe(GuiElement&);
    void Render(const Scene& scene, const Time&);

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    std::vector<std::unique_ptr<IGuiRenderer>> renderers_;
};
}  // namespace GameEngine
