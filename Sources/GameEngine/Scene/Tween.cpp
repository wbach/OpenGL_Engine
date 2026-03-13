#include "Tween.h"

#include "Logger/Log.h"

namespace GameEngine
{
bool Tween::Update(float dt)
{
    elapsed += dt;

    auto t = std::clamp(elapsed / duration, 0.0f, 1.0f);

    auto easedT = ApplyEase(t, easeType);

    auto newPos   = glm::mix(source.position, target.position, easedT);
    auto newRot   = glm::slerp(source.rotation.value_, target.rotation.value_, easedT);
    auto newScale = glm::mix(source.scale, target.scale, easedT);

    gameObject.get().SetWorldPositionRotationScale(newPos, newRot, newScale);

    if (t >= 1.0f)
    {
        if (onComplete)
        {
            onComplete();
        }

        return true;
    }
    return false;
}
float Tween::ApplyEase(float t, EaseType type)
{
    switch (type)
    {
        case EaseType::CubicOut:
            return 1.0f - pow(1.0f - t, 3.0f);
        case EaseType::QuadInOut:
            return t < 0.5f ? 2.0f * t * t : 1.0f - pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
        default:
            return t;
    }
}
}  // namespace GameEngine
