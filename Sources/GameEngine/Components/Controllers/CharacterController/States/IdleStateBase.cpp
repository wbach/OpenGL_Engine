#include "IdleStateBase.h"

#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"

namespace GameEngine
{
namespace Components
{
IdleStateBase::IdleStateBase(FsmContext &context, const std::string &idleAnimName,
                             std::optional<std::string> jointGroupName)
    : context_{context}
    , idleAnimName_{idleAnimName}
    , jointGroupName_{jointGroupName}
{
}

void IdleStateBase::onEnter(const CrouchChangeStateEvent &)
{
    setIdleAnim();
}
void IdleStateBase::onEnter(const EquipEndStateEvent &)
{
    setIdleAnim();
}
void IdleStateBase::onEnter(const DisarmEndStateEvent &)
{
    setIdleAnim();
}
void IdleStateBase::onEnter(const AimStopEvent &)
{
    setIdleAnim();
}

void IdleStateBase::onEnter(const EndAttackEvent &)
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
                                          jointGroupName_);
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

void IdleStateBase::onEnter(const EndMoveLeftEvent &)
{
    setIdleAnim();
}

void IdleStateBase::onEnter(const EndMoveRightEvent &)
{
    setIdleAnim();
}
}  // namespace Components
}  // namespace GameEngine
