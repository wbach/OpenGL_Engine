#include "ChaseState.h"

#include <Utils/GLM/GLMUtils.h>

#include "../CombatStateEvents.h"
#include "GameEngine/Components/Controllers/AI/AIController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Navigation/NavigationManager.h"

namespace GameEngine
{
namespace Components
{
ChaseState::ChaseState(CombatStateContext& combatStateContext)
    : combatStateContext{combatStateContext}
{
}
void ChaseState::onEnter()
{
}
void ChaseState::onEnter(const TargetOutOfRange& event)
{
    target = event.target;
}
void ChaseState::update(float deltaTime)
{
    if (not target)
    {
        combatStateContext.controller.pushEventToQueue(TargetLost{});
        return;
    }

    const auto currentPos = combatStateContext.gameObject.GetWorldTransform().GetPosition();
    const auto targetPos  = target->GetWorldTransform().GetPosition();

    const auto distance = glm::distance(currentPos, targetPos);

    if (distance <= combatStateContext.controller.getMeleAttackRadius())
    {
        LOG_DEBUG << "InAttack range";
        combatStateContext.controller.pushEventToQueue(TargetInMeleeRangeEvent{.target = target});
        return;
    }
    auto& currentPath = combatStateContext.currentPath;

    pathUpdateTimer_ -= deltaTime;

    if (not combatStateContext.navigationManager.Raycast(currentPos, targetPos))
    {
        currentPath.clear();
        moveTo(targetPos);
    }
    else if (pathUpdateTimer_ <= 0.f)
    {
        pathUpdateTimer_ = (distance > 20.f) ? 0.6f : 0.3f;
        currentPath      = combatStateContext.navigationManager.CalculatePath(currentPos, targetPos);

        if (currentPath.size() > 1 and glm::distance(currentPos, currentPath[0]) < 0.5f)
        {
            currentPath.erase(currentPath.begin());
        }

        if (not currentPath.empty())
        {
            moveTo(currentPath[0]);
        }
    }
    else
    {
        if (not currentPath.empty())
        {
            moveTo(currentPath[0]);

            if (glm::distance(currentPos, currentPath[0]) < 0.5f)
            {
                currentPath.erase(currentPath.begin());
            }
        }
        else
        {
            pathUpdateTimer_ = 0.f;
        }
    }
}
void ChaseState::moveTo(const vec3& targetPos)
{
    combatStateContext.characterController.pushEventToQueue(MoveForwardEvent{});
    isMoving = true;

    const auto currentPos = combatStateContext.gameObject.GetWorldTransform().GetPosition();

    auto direction = targetPos - currentPos;
    direction.y    = 0.0f;

    if (glm::length(direction) > 0.0001f)
    {
        auto targetRotation = Utils::calculateTargetRotation(glm::normalize(direction));
        combatStateContext.characterController.pushEventToQueue(RotateTargetEvent{targetRotation});
    }
}
}  // namespace Components
}  // namespace GameEngine
