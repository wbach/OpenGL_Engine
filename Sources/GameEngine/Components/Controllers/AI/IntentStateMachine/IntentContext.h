#pragma once

namespace GameEngine
{
class GameObject;

namespace Components
{
class RoutineComponent;
class AIController;

struct IntentContext
{
    RoutineComponent* routineComponent{nullptr};
    GameObject& gameObject;
    AIController& controller;
};
}  // namespace Components
}  // namespace GameEngine
