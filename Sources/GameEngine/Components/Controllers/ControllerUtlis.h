#pragma once
#include <Types.h>
#include "GameEngine/Components/ComponentController.h"

namespace GameEngine
{
namespace Components
{
template <class T>
std::tuple<float, vec3, T*> getComponentsInRange(const ComponentController& componentController,
                                                 const vec3& sourcePosition)
{
    auto& components = componentController.GetAllComonentsOfType(T::type);

    T* closestComponent{nullptr};
    vec3 vectorToTarget{0.f};

    float distance = std::numeric_limits<float>::max();

    for (const auto& component : components)
    {
        const auto& targetPosition = component.second->getParentGameObject().GetWorldTransform().GetPosition();

        const auto dist              = targetPosition - sourcePosition;
        const auto& distanceToPlayer = glm::length(dist);

        if (distanceToPlayer < distance)
        {
            distance         = distanceToPlayer;
            vectorToTarget   = dist;
            closestComponent = static_cast<T*>(component.second);
        }
    }
    return {distance, vectorToTarget, closestComponent};
}
}  // namespace Components
}  // namespace GameEngine
