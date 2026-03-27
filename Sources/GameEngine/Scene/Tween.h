#pragma once
#include <Common/Transform.h>
#include <EngineApi.h>

#include <functional>

#include "EaseType.h"
#include "GameEngine/Objects/GameObject.h"
#include "TweenTransform.h"
namespace GameEngine
{
namespace Components
{
class Rigidbody;
}
struct ENGINE_API Tween
{
    bool Update(float);
    float ApplyEase(float, EaseType);
    void ApplyToTransform(const TweenTransform&);
    void ApplyToRigidbodyTransform(const TweenTransform&, Components::Rigidbody&);
    void ApplyToEngineTransform(const TweenTransform&);
    TweenTransform calulacteTransform(float) const;

    std::reference_wrapper<GameObject> gameObject;
    common::TransformContext source;
    TweenTransform target;
    EaseType easeType;
    std::function<void()> onComplete;
    float duration = 0.f;
    float elapsed  = 0.0f;
};
}  // namespace GameEngine
