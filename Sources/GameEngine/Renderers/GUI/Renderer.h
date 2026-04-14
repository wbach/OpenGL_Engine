#pragma once
#include <memory>
#include <vector>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "IGuiRenderer.h"
#include "RenderAble.h"

namespace GameEngine
{
struct Time;

namespace GUI
{
class ENGINE_API Renderer
{
public:
    Renderer(GraphicsApi::IGraphicsApi&);
    ~Renderer();

    void Init();
    void UnSubscribeAll();
    void UnSubscribe(uint32);
    void UnSubscribe(const Element&);
    void ReloadShaders();
    void Subscribe(Element&);
    void render();

private:
    void SortSubscribers();

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;

    ShaderProgram shader_;
    std::vector<RenderAble*> subscribers_;

    bool isInit_;
    uint32 colorBuffer_;
    uint32 transformBuffer_;
};
}  // namespace GUI
}  // namespace GameEngine
