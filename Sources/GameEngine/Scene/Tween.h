#pragma once
#include <EngineApi.h>

#include <functional>

#include "Common/Transform.h"
#include "EaseType.h"
#include "GameEngine/Objects/GameObject.h"
#include "Rotation.h"

namespace GameEngine
{
struct ENGINE_API Tween
{
    bool Update(float);
    float ApplyEase(float, EaseType);

    std::reference_wrapper<GameObject> gameObject;
    common::TransformContext source;
    common::TransformContext target;
    EaseType easeType;
    std::function<void()> onComplete;
    float duration = 0.f;
    float elapsed  = 0.0f;
};
}  // namespace GameEngine
