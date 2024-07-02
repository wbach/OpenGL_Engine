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
    AttackStateBase(FsmContext&, const std::vector<AttackAnimation>&);

    void onEnter(const AttackEvent&);
    void update(const AttackEvent&);
    void update(float);
    void onLeave();

private:
    void onClipEnd();
    void unsubscribe();

private:
    FsmContext& context;

    std::vector<IdType> subIds;
    const std::vector<AttackAnimation>& attackClipNames;

    uint32 sequenceSize = 0;
    uint32 currentAnimation = 0;
};
}  // namespace Components
}  // namespace GameEngine
