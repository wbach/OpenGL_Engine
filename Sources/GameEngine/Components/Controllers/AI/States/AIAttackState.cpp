#include "AIAttackState.h"

#include <Variant.h>

#include "../AIController.h"
#include "../AIControllerContext.h"
#include "../AIControllerEvents.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
AIAttackState::AIAttackState(AIControllerContext& context)
    : context{context}

{
}
AIAttackState::~AIAttackState()
{
}
void AIAttackState::update(float)
{
}
void AIAttackState::onEnter()
{
    characterControllerEventSubId = context.characterController.subscribeForEvent(
        [this](const auto& event) {
            std::visit(visitor{[&](const EndAttackEvent& endAttackEvent) { process(endAttackEvent); }, [&](const auto&) {}},
                       event);
        });

    context.characterController.pushEventToQueue(AttackEvent{});
}
void AIAttackState::onEnter(const TargetInAttackRangeEvent& event)
{
    target = &event.target;
}
void AIAttackState::process(const EndAttackEvent&)
{
    if (not target)
        return;

    const auto& currentPos = context.gameObject.GetWorldTransform().GetPosition();
    const auto& targetPos  = target->GetWorldTransform().GetPosition();
    const auto distance    = glm::distance(currentPos, targetPos);

    if (distance <= context.controller.getAttackRadius())
    {
        context.characterController.pushEventToQueue(AttackEvent{});
    }
    else
    {
        context.controller.pushEventToQueue(TargetOutOfAttackRangeEvent{});
    }
}
void AIAttackState::onLeave()
{
    context.characterController.unsubscribeForEvent(characterControllerEventSubId);
    target = nullptr;
}
}  // namespace Components
}  // namespace GameEngine
