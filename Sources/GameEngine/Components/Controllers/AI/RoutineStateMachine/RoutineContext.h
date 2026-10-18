#pragma once

namespace GameEngine
{
class GameObject;

namespace Components
{
class ProfessionComponent;

struct RoutineContext
{
    ProfessionComponent* profession_{nullptr};
    GameObject& gameObject;
};
}  // namespace Components
}  // namespace GameEngine
