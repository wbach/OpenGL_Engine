#pragma once
#include <memory>
#include <vector>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererFunctionType.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "GuiRendererElementBase.h"
#include "IGuiRenderer.h"
#include "Logger/Log.h"

namespace GameEngine
{
struct Time;

class GUIRenderer
{
public:
    GUIRenderer(GraphicsApi::IGraphicsApi& graphicsApi);
    ~GUIRenderer();
    void Init();
    void UnSubscribeAll();
    void UnSubscribe(uint32);
    void UnSubscribe(const GuiElement&);
    void ReloadShaders();
    void Subscribe(GuiElement&);
    void Render(const Scene& scene, const Time&);

private:
    void SortSubscribers();

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;

    ShaderProgram shader_;
    std::vector<GuiRendererElementBase*> subscribers_;

    bool isInit_;
    uint32 colorBuffer_;
    uint32 transformBuffer_;
};
}  // namespace GameEngine
