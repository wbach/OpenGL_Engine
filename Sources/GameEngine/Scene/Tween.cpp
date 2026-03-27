#include "Tween.h"

#include <Logger/Log.h>
#include <Rotation.h>

#include <optional>

#include "GameEngine/Components/Physics/Rigidbody.h"

namespace GameEngine
{
bool Tween::Update(float dt)
{
    elapsed += dt;

    auto t = std::clamp(elapsed / duration, 0.0f, 1.0f);
    ApplyToTransform(calulacteTransform(ApplyEase(t, easeType)));

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
void Tween::ApplyToTransform(const TweenTransform& newTransform)
{
    if (auto rigidbody = gameObject.get().GetComponent<Components::Rigidbody>())
    {
        ApplyToRigidbodyTransform(newTransform, *rigidbody);
    }
    else
    {
        ApplyToEngineTransform(newTransform);
    }
}
TweenTransform Tween::calulacteTransform(float easedT) const
{
    TweenTransform newTransform;
    if (target.position)
    {
        newTransform.position = glm::mix(source.position, target.position.value(), easedT);
    }
    if (target.rotation)
    {
        newTransform.rotation = glm::slerp(source.rotation.value_, target.rotation->value_, easedT);
    }
    if (target.scale)
    {
        newTransform.scale = glm::mix(source.scale, target.scale.value(), easedT);
    }

    return newTransform;
}
void Tween::ApplyToRigidbodyTransform(const TweenTransform& newTransform, Components::Rigidbody& rigidbody)
{
    if (newTransform.position)
    {
        rigidbody.SetPosition(*newTransform.position);
    }
    if (newTransform.rotation)
    {
        rigidbody.SetRotation(*newTransform.rotation);
    }
    if (newTransform.scale)
    {
        rigidbody.SetScale(*newTransform.scale);
    }
}
void Tween::ApplyToEngineTransform(const TweenTransform& newTransform)
{
    if (newTransform.position and newTransform.rotation and newTransform.scale)
    {
        gameObject.get().SetWorldPositionRotationScale(*newTransform.position, *newTransform.rotation, *newTransform.scale);
    }
    else if (newTransform.position and newTransform.rotation)
    {
        gameObject.get().SetWorldPositionRotation(*newTransform.position, *newTransform.rotation);
    }
    else
    {
        if (newTransform.position)
        {
            gameObject.get().SetWorldPosition(*newTransform.position);
        }

        if (newTransform.rotation)
        {
            gameObject.get().SetWorldRotation(*newTransform.rotation);
        }
        if (newTransform.scale)
        {
            gameObject.get().SetWorldScale(*newTransform.scale);
        }
    }
}
}  // namespace GameEngine
