#include "ArmedRunState.h"

#include "../CharacterController.h"
#include "DisarmedRunState.h"

namespace GameEngine
{
namespace Components
{
ArmedRunState::ArmedRunState(FsmContext &context)
    : MoveStateBase{context, std::nullopt, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
{
}

void ArmedRunState::onMoveInactivity()
{
    context_.animator.ChangeAnimation(context_.animClipNames.armed.idle, Animator::AnimationChangeType::smooth,
                                      PlayDirection::forward, jointGroupName_);
}
}  // namespace Components
}  // namespace GameEngine
