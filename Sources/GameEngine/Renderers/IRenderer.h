#pragma once

namespace GameEngine
{
class GameObject;
namespace Components
{
class IComponent;
}

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
    virtual void unSubscribe(const Components::IComponent&);
    virtual void unSubscribeAll();
    virtual void cleanUp();
};
}  // namespace GameEngine
