#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "IdleStateBase.h"

namespace GameEngine
{
namespace Components
{
class ArmedRunState;
class ArmedRotateState;
class DrawArrowState;
class DisarmedCrouchState;
class DisarmedIdleState;
class JumpState;
class DeathState;
class IdleArmedChangeState;

class AttackStateBase
{
public:
    AttackStateBase(FsmContext&, const std::vector<std::string>&);

    void onEnter(const AttackEvent&);
    void update(float);
    void onLeave();

private:
    FsmContext& context;

    std::vector<IdType> subIds;
    const std::vector<std::string>& attackClipNames;
};
}  // namespace Components
}  // namespace GameEngine
