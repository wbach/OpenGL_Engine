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
    if (not context_.animClipNames.disarmed.death.empty())
    {
        context_.animator.ChangeAnimation(context_.animClipNames.disarmed.death, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, std::nullopt);
    }
}

void DeathState::update(float)
{
}

}  // namespace Components
}  // namespace GameEngine
