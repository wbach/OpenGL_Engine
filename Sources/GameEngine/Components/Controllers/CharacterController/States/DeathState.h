#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"

namespace GameEngine
{
namespace Components
{
class DisarmedIdleState;
class DisarmedRunState;
class DisarmedRotateState;
class DisarmedRunAndRotateState;
class DisarmedWalkState;
class DisarmedWalkAndRotateState;
class DisarmedSprintState;
class DisarmedSprintAndRotateState;
class ArmedIdleState;
class ArmedRunState;
class ArmedRotateState;
class ArmedRunAndRotateState;
class ArmedWalkState;
class ArmedWalkAndRotateState;
class ArmedSprintState;
class ArmedSprintAndRotateState;
class JumpState;
class MoveJumpState;
class AimState;
class AimRotateState;
class AimWalkState;
class AimWalkAndRotateState;
class RecoilState;
class RecoilRotateState;
class RecoilWalkState;
class RecoilWalkAndRotateState;
class DrawArrowState;
class DrawArrowRotateState;
class DrawArrowWalkState;
class DrawArrowWalkAndRotateState;
class IdleArmedChangeState;
class RotateArmedChangeState;
class RunArmedChangeState;
class RunAndRotateArmedChangeState;
class WalkArmedChangeState;
class WalkAndRotateArmedChangeState;
class DisarmedAttackState;

class DeathState : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    DeathState(FsmContext&);
    void update(float);

    void onEnter(DisarmedAttackState&, const DeathEvent&);
    void onEnter(IdleArmedChangeState&, const DeathEvent&);
    void onEnter(RotateArmedChangeState&, const DeathEvent&);
    void onEnter(RunArmedChangeState&, const DeathEvent&);
    void onEnter(RunAndRotateArmedChangeState&, const DeathEvent&);
    void onEnter(WalkArmedChangeState&, const DeathEvent&);
    void onEnter(WalkAndRotateArmedChangeState&, const DeathEvent&);
    void onEnter(DisarmedIdleState&, const DeathEvent&);
    void onEnter(DisarmedRunState&, const DeathEvent&);
    void onEnter(DisarmedRotateState&, const DeathEvent&);
    void onEnter(DisarmedRunAndRotateState&, const DeathEvent&);
    void onEnter(DisarmedWalkState&, const DeathEvent&);
    void onEnter(DisarmedWalkAndRotateState&, const DeathEvent&);
    void onEnter(DisarmedSprintState&, const DeathEvent&);
    void onEnter(DisarmedSprintAndRotateState&, const DeathEvent&);
    void onEnter(ArmedIdleState&, const DeathEvent&);
    void onEnter(ArmedRunState&, const DeathEvent&);
    void onEnter(ArmedRotateState&, const DeathEvent&);
    void onEnter(ArmedRunAndRotateState&, const DeathEvent&);
    void onEnter(ArmedWalkState&, const DeathEvent&);
    void onEnter(ArmedWalkAndRotateState&, const DeathEvent&);
    void onEnter(ArmedSprintState&, const DeathEvent&);
    void onEnter(ArmedSprintAndRotateState&, const DeathEvent&);
    void onEnter(JumpState&, const DeathEvent&);
    void onEnter(MoveJumpState&, const DeathEvent&);
    void onEnter(AimState&, const DeathEvent&);
    void onEnter(AimRotateState&, const DeathEvent&);
    void onEnter(AimWalkState&, const DeathEvent&);
    void onEnter(AimWalkAndRotateState&, const DeathEvent&);
    void onEnter(RecoilState&, const DeathEvent&);
    void onEnter(RecoilRotateState&, const DeathEvent&);
    void onEnter(RecoilWalkState&, const DeathEvent&);
    void onEnter(RecoilWalkAndRotateState&, const DeathEvent&);
    void onEnter(DrawArrowState&, const DeathEvent&);
    void onEnter(DrawArrowRotateState&, const DeathEvent&);
    void onEnter(DrawArrowWalkState&, const DeathEvent&);
    void onEnter(DrawArrowWalkAndRotateState&, const DeathEvent&);

private:
    void setAnim(const std::string&);

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
