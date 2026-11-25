#include "CombatState.h"

#include <GameEngine/Components/Controllers/AI/AIController.h>
#include <GameEngine/Objects/GameObject.h>
#include <Logger/Log.h>

#include "../../ReactionStateContext.h"
#include "CombatStateEvents.h"

namespace GameEngine
{
namespace Components
{
CombatState::CombatState(ReactionStateContext& context)
    : reactionStateContext{context}
    , combatStateContext{new CombatStateContext{.gameObject          = reactionStateContext.gameObject,
                                                .controller          = reactionStateContext.controller,
                                                .navigationManager   = reactionStateContext.navigationManager,
                                                .characterController = reactionStateContext.characterController,
                                                .currentPath         = reactionStateContext.currentPath}}

{
}
CombatState::~CombatState()
{
}
void CombatState::onEnter()
{
    LOG_DEBUG << "creating CombatStateMachine...";
    combatStateMachine = std::make_unique<CombatStateMachine>(CombatIdleState{}, ChaseState{*combatStateContext},
                                                              AttackState{*combatStateContext}, DefenseState{});
}
void CombatState::onEnter(const DamageTaken& event)
{
    LOG_DEBUG << "DamageTaken";
    if (not event.target)
    {
        LOG_WARN << "Target is empty!";
        reactionStateContext.controller.pushEventToQueue(TargetLostEvent{});
        return;
    }

    onEnter(*event.target);
}
void CombatState::onEnter(const TargetSpotted& event)
{
    if (not event.target)
    {
        reactionStateContext.controller.pushEventToQueue(TargetLostEvent{});
        return;
    }

    onEnter(*event.target);
}
void CombatState::update(float)
{
}
void CombatState::onLeave(const TargetLost&)
{
    reactionStateContext.controller.pushEventToQueue(ThreatEnded{});
}
void CombatState::onLeave(const ThreatCleared&)
{
    reactionStateContext.controller.pushEventToQueue(ThreatEnded{});
}
void CombatState::onLeave()
{
    combatStateMachine.reset();
}
void CombatState::onEnter(GameObject& target)
{
    LOG_DEBUG << "onEnter";
    const auto currentPos = reactionStateContext.gameObject.GetWorldTransform().GetPosition();
    LOG_DEBUG << "currentPos:  "<<  currentPos;
    const auto targetPos  = target.GetWorldTransform().GetPosition();
LOG_DEBUG << "targetPos:  "<<  targetPos;
    const auto distance = glm::distance(currentPos, targetPos);

LOG_DEBUG << "distance:  "<<  distance;
    if (distance <= reactionStateContext.controller.getMeleAttackRadius())
    {
        LOG_DEBUG << "InAttack range : " << target.GetName();
        combatStateMachine->handle(TargetInMeleeRangeEvent{});
        return;
    }

    auto maybeRangeAttackRadius = reactionStateContext.controller.getRangeAttackRadius();
    if (maybeRangeAttackRadius and distance <= maybeRangeAttackRadius.value())
    {
        LOG_DEBUG << "InAttack range : " << target.GetName();
        combatStateMachine->handle(TargetInRangeAttackRangeEvent{});
        return;
    }

    LOG_DEBUG << "TargetOutOfRange : " << target.GetName();
    combatStateMachine->handle(TargetOutOfRange{});
}
void CombatState::update(const CombatStateEvent& combatStateEvent)
{
    auto passEventToMachine = [&](const auto& event)
    {
        LOG_DEBUG << Utils::GetTypeName(event);
        combatStateMachine->handle(event);
    };
    std::visit(passEventToMachine, combatStateEvent);
}
}  // namespace Components
}  // namespace GameEngine
