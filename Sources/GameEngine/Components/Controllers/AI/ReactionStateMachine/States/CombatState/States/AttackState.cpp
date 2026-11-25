#include "AttackState.h"

#include <Utils.h>
#include <Variant.h>

#include "GameEngine/Components/Controllers/AI/AIController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
AttackState::AttackState(CombatStateContext& combatStateContext)
    : combatStateContext{combatStateContext}
{
}
void AttackState::onEnter()
{
    characterControllerEventSubId = combatStateContext.characterController.subscribeForEvent(
        [this](const auto& event)
        {
            std::visit(visitor{[&](const EndAttackEvent& endAttackEvent) { process(endAttackEvent); },
                               [&](const auto& e) { LOG_DEBUG << Utils::GetTypeName(e); }},
                       event);
        });

    combatStateContext.characterController.pushEventToQueue(AttackEvent{});
}
void AttackState::onEnter(const TargetInMeleeRangeEvent& event)
{
    target = event.target;
}
void AttackState::onEnter(const TargetInRangeAttackRangeEvent& event)
{
    target = event.target;
}
void AttackState::update(float)
{
}
void AttackState::process(const EndAttackEvent&)
{
    if (not target)
        return;

    const auto& currentPos = combatStateContext.gameObject.GetWorldTransform().GetPosition();
    const auto& targetPos  = target->GetWorldTransform().GetPosition();
    const auto distance    = glm::distance(currentPos, targetPos);

    if (distance <= combatStateContext.controller.getMeleAttackRadius())
    {
        LOG_DEBUG << "mele attack";
        combatStateContext.characterController.pushEventToQueue(AttackEvent{});
        return;
    }

    auto maybeRangeAttackRadius = combatStateContext.controller.getRangeAttackRadius();
    if (maybeRangeAttackRadius and distance <= maybeRangeAttackRadius.value())
    {
        LOG_DEBUG << "Range attack";
        combatStateContext.characterController.pushEventToQueue(AttackEvent{});
        return;
    }

    combatStateContext.controller.pushEventToQueue(TargetOutOfRange{});
}
void AttackState::onLeave()
{
    combatStateContext.characterController.unsubscribeForEvent(characterControllerEventSubId);
    target = nullptr;
}

}  // namespace Components
}  // namespace GameEngine
