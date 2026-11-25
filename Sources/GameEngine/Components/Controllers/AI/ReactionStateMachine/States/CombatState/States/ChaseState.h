#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CombatStateContext.h"

namespace GameEngine
{
class GameObject;

namespace Components
{
struct TargetOutOfRange;

class ChaseState : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    ChaseState(CombatStateContext&);

    void onEnter();
    void onEnter(const TargetOutOfRange&);
    void update(float);

private:
    void moveTo(const vec3&);

private:
    CombatStateContext& combatStateContext;
    GameObject* target{nullptr};
    float pathUpdateTimer_{-1.f};
    bool isMoving{false};
};
}  // namespace Components
}  // namespace GameEngine
