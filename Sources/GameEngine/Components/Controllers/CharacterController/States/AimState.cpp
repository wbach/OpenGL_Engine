#include "AimState.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimState::AimState(FsmContext &context)
    : context_{context}
{
}
void AimState::onEnter(const AimStartEvent &)
{
    DEBUG_LOG("onEnter AimStartEvent clip : " + context_.animClipNames.aimIdle);
    setAnim();
}

void AimState::setAnim()
{
    context_.animator.ChangeAnimation(
        context_.animClipNames.aimIdle, Animator::AnimationChangeType::smooth, PlayDirection::forward,
        context_.multiAnimations ? std::make_optional(context_.upperBodyGroupName) : std::nullopt);
}

}  // namespace Components
}  // namespace GameEngine
