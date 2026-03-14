#pragma once
#include <EngineApi.h>

#include <functional>
#include <optional>
#include <type_traits>
#include <vector>

#include "Rotation.h"
#include "Tween.h"

namespace GameEngine
{
class ENGINE_API TweenManager
{
public:
    void Update(float);
    void Add(GameObject&, const TweenTransform&, float, EaseType, std::function<void()> = nullptr);
    void Remove(const GameObject&);

private:
    std::vector<Tween> activeTweens;
};
}  // namespace GameEngine
