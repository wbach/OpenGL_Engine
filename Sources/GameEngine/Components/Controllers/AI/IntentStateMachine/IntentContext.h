#pragma once

namespace GameEngine
{
class GameObject;

namespace Components
{
class RoutineComponent;

struct IntentContext
{
    RoutineComponent* routineComponent{nullptr};
    GameObject& gameObject;
};
}  // namespace Components
}  // namespace GameEngine
