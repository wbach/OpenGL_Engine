#pragma once
#include <vector>

#include "ITweenManager.h"
#include "Tween.h"

namespace GameEngine
{
class ENGINE_API TweenManager : public ITweenManager
{
public:
    void Update(float) override;
    void Add(GameObject&, const TweenTransform&, float, EaseType, std::function<void()> = nullptr) override;
    void Remove(const GameObject&) override;

private:
    std::vector<Tween> activeTweens;
};
}  // namespace GameEngine
