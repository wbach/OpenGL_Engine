#pragma once

namespace GameEngine
{
class GameObject;

class IRenderer
{
public:
    virtual ~IRenderer();
    virtual void init();
    virtual void reloadShaders();
    virtual void prepare();
    virtual void render();
    virtual void blendRender();
    virtual void subscribe(GameObject&);
    virtual void unSubscribe(GameObject&);
    virtual void unSubscribeAll();
    virtual void cleanUp();
};
}  // namespace GameEngine
