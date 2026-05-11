#include "HurtState.h"

#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"

namespace GameEngine
{
namespace Components
{
HurtState::HurtState(FsmContext& context)
    : BaseState(context)
{
}
void HurtState::onEnter()
{
    const auto& animName = context_.animClipNames.disarmed.posture.stand.hurt;
    animationSubId =
        context_.animator.SubscribeForAnimationFrame(animName,
                                                     [this]()
                                                     {
                                                         animationSubId.reset();
                                                         context_.characterController.pushEventToQueue(AnimationPlayEndEvent{});
                                                     });

    context_.animator.ChangeAnimation(animName);
}
bool HurtState::entryCondition() const
{
    return not context_.animClipNames.disarmed.posture.stand.hurt.empty();
}
void HurtState::onLeave()
{
    flushEvents();
}
}  // namespace Components
}  // namespace GameEngine
