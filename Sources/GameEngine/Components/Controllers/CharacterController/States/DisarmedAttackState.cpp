#include "DisarmedAttackState.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"

namespace GameEngine
{
namespace Components
{
DisarmedAttackState::DisarmedAttackState(FsmContext &context)
    : context{context}
{
}
void DisarmedAttackState::onEnter(const AttackEvent &)
{
    DEBUG_LOG("DisarmedAttackState::onEnter(const AttackEvent &)");
    const auto &attackClipNames = context.animClipNames.disarmed.attack;

    if (not attackClipNames.empty())
    {
        const auto &clipName = attackClipNames.front();
        DEBUG_LOG(clipName);
        context.animator.ChangeAnimation(clipName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                         std::nullopt);

        auto subId = context.animator.SubscribeForAnimationFrame(
            clipName, [&]() { context.characterController.pushEventToQueue(EndAttackEvent{}); });
        subIds.push_back(subId);
    }
}
void DisarmedAttackState::update(float)
{
}
void DisarmedAttackState::onLeave()
{
    if (not subIds.empty())
    {
        for (const auto &id : subIds)
            context.animator.UnSubscribeForAnimationFrame(id);
        subIds.clear();
    }
}
}  // namespace Components
}  // namespace GameEngine
