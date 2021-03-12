#include "DeathState.h"

namespace GameEngine
{
namespace Components
{
DeathState::DeathState(FsmContext &context)
    : context_{context}
{
}

void DeathState::onEnter()
{
    DEBUG_LOG("onEnter()");
    if (not context_.diedAnimationName.empty())
    {
        context_.animator.ChangeAnimation(context_.diedAnimationName, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, std::nullopt);
    }
}

void DeathState::update(float)
{
}

}  // namespace Components
}  // namespace GameEngine
