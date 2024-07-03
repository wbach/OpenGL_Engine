#include "DisarmedRunState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedRunState::DisarmedRunState(FsmContext &context)
    : RotatingMoveState{context, std::nullopt, context.runSpeed.forward, context.animClipNames.disarmed.run.forward}
{
}
void DisarmedRunState::onMoveInactivity()
{
    context_.animator.ChangeAnimation(context_.animClipNames.disarmed.idle, Animator::AnimationChangeType::smooth,
                                      PlayDirection::forward, jointGroupName_);
}
}  // namespace Components
}  // namespace GameEngine
