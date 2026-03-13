#pragma once
#include <EngineApi.h>

#include <functional>
#include <type_traits>
#include <vector>

#include "Tween.h"

namespace GameEngine
{
class ENGINE_API TweenManager
{
public:
    void Update(float);
    void Add(GameObject&, const common::TransformContext&, float, EaseType, std::function<void()>);
    void Remove(const GameObject&);

private:
    std::vector<Tween> activeTweens;
};
}  // namespace GameEngine
