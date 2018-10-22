#pragma once
#include <memory>
#include <vector>
#include "Types.h"

namespace GameEngine
{
class GameObject;
class Scene;

struct IRenderer
{
    virtual ~IRenderer();
    virtual void Init()          = 0;
    virtual void ReloadShaders() = 0;
    virtual void Subscribe(GameObject* gameObject);
    virtual void UnSubscribe(GameObject* gameObject);
    virtual void UnSubscribeAll();
};

typedef std::unique_ptr<IRenderer> IRendererPtr;
typedef std::vector<IRendererPtr> IRenderersPtrVec;
}  // GameEngine
