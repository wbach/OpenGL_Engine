#include "DisarmedRunState.h"

namespace GameEngine
{
namespace Components
{
DisarmedRunState::DisarmedRunState(FsmContext &context)
    : MoveStateBase{context, std::nullopt, context.runSpeed, context.animClipNames.disarmed.run, context.animClipNames.disarmed.idle}
{
}

void DisarmedRunState::onMoveInactivity()
{
    context_.animator.ChangeAnimation(context_.animClipNames.armed.idle, Animator::AnimationChangeType::smooth,
                                      PlayDirection::forward, jointGroupName_);
}
}  // namespace Components
}  // namespace GameEngine
