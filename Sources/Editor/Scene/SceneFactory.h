#pragma once
#include "Scene/SceneFactoryBase.h"

namespace Editor
{
struct Context;

class SceneFactory : public GameEngine::SceneFactoryBase
{
public:
    SceneFactory(Context&);
};
}  // namespace Editor
