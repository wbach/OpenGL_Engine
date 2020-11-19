#pragma once

namespace GameEngine
{
class GameObject;

struct IRenderer
{
    virtual ~IRenderer();
    virtual void init();
    virtual void reloadShaders();
    virtual void prepare();
    virtual void render();
    virtual void blendRender();
    virtual void subscribe(GameObject&);
    virtual void unSubscribe(GameObject&);
    virtual void unSubscribeAll();
};
}  // namespace GameEngine
