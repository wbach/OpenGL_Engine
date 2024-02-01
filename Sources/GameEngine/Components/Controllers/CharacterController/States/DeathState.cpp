#include "DeathState.h"

namespace GameEngine
{
namespace Components
{
DeathState::DeathState(FsmContext &context)
    : context_{context}
{
}

void DeathState::update(float)
{
}

void DeathState::onEnter(DisarmedIdleState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.death);
}

void DeathState::onEnter(DisarmedRunState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.death);
}

void DeathState::onEnter(DisarmedRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.death);
}

void DeathState::onEnter(DisarmedRunAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.death);
}

void DeathState::onEnter(DisarmedWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.death);
}

void DeathState::onEnter(DisarmedWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.death);
}

void DeathState::onEnter(DisarmedSprintState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.death);
}

void DeathState::onEnter(DisarmedSprintAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.death);
}

void DeathState::onEnter(ArmedIdleState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(ArmedRunState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(ArmedRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(ArmedRunAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(ArmedWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(ArmedWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(ArmedSprintState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(ArmedSprintAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(JumpState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(MoveJumpState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(AimState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(AimRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(AimRunState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(AimRunAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(AimWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(AimWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(RecoilState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(RecoilRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(RecoilRunState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(RecoilRunAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(RecoilWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(RecoilWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(DrawArrowState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(DrawArrowRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(DrawArrowRunState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(DrawArrowRunAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(DrawArrowWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::onEnter(DrawArrowWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.death);
}

void DeathState::setAnim(const std::string &animName)
{
    context_.multiAnimations = false;
    if (not animName.empty())
    {
        context_.animator.ChangeAnimation(animName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                          std::nullopt);
    }
}

}  // namespace Components
}  // namespace GameEngine
