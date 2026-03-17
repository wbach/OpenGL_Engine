#pragma once
#include <EngineApi.h>

#include <functional>

#include "EaseType.h"
#include "TweenTransform.h"

namespace GameEngine
{
class GameObject;
class ENGINE_API ITweenManager
{
public:
    virtual ~ITweenManager() = default;

    virtual void Update(float)                                                                             = 0;
    virtual void Add(GameObject&, const TweenTransform&, float, EaseType, std::function<void()> = nullptr) = 0;
    virtual void Remove(const GameObject&)                                                                 = 0;
};
}  // namespace GameEngine
