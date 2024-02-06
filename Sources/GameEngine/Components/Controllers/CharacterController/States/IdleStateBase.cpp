#include "IdleStateBase.h"

#include "../Attack/AttackEvents.h"
#include "GameEngine/Components/Animation/Animator.h"

namespace GameEngine
{
namespace Components
{
IdleStateBase::IdleStateBase(FsmContext &context, const std::string &idleAnimName)
    : context_{context}
    , idleAnimName_{idleAnimName}
{
}
void IdleStateBase::onEnter(const EquipEndStateEvent&)
{
    setIdleAnim();
}
void IdleStateBase::onEnter(const DisarmEndStateEvent&)
{
    setIdleAnim();
}
void IdleStateBase::onEnter(const AimStopEvent &)
{
    setIdleAnim();
}

void IdleStateBase::update(const AttackEvent &)
{
}

void IdleStateBase::update(const EndAttackEvent &)
{
    setIdleAnim();
}

void IdleStateBase::update(float)
{
}

void IdleStateBase::setIdleAnim()
{
    if (not idleAnimName_.empty())
    {
        context_.animator.ChangeAnimation(idleAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                          std::nullopt);
    }
}

void IdleStateBase::onEnter(const EndForwardMoveEvent &)
{
    setIdleAnim();
}

void IdleStateBase::onEnter(const EndBackwardMoveEvent &)
{
    setIdleAnim();
}

void IdleStateBase::onEnter(const EndRotationEvent &)
{
    setIdleAnim();
}
}  // namespace Components
}  // namespace GameEngine
