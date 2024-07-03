#include "ArmedRunState.h"

#include "../CharacterController.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedRunState::ArmedRunState(FsmContext &context)
    : RotatingMoveState{context, std::nullopt, context.runSpeed.forward, context.animClipNames.armed.run.forward}
{
}

void ArmedRunState::onMoveInactivity()
{
    context_.animator.ChangeAnimation(context_.animClipNames.armed.idle, Animator::AnimationChangeType::smooth,
                                      PlayDirection::forward, jointGroupName_);
}
}  // namespace Components
}  // namespace GameEngine
