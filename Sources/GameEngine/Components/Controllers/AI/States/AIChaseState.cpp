#include "AIChaseState.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

#include "../AIController.h"
#include "../AIControllerContext.h"
#include "GameEngine/Components/Controllers/AI/AIControllerEvents.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Navigation/NavigationManager.h"
namespace GameEngine
{
namespace Components
{
AIChaseState::AIChaseState(AIControllerContext& context)
    : context_{context}
{
}
void AIChaseState::onEnter(const TargetSpottedEvent& event)
{
    LOG_DEBUG << context_.gameObject.GetName() << " spot " << event.target.GetName();
    target = &event.target;
    context_.currentPath.clear();
}
void AIChaseState::update(float deltaTime)
{
    if (not target)
    {
        context_.controller.pushEventToQueue(TargetLostEvent{});
        context_.characterController.pushEventToQueue(EndForwardMoveEvent{});
        return;
    }

    const auto currentPos = context_.gameObject.GetWorldTransform().GetPosition();
    const auto targetPos  = target->GetWorldTransform().GetPosition();

    const auto distance = glm::distance(currentPos, targetPos);

    if (distance > context_.controller.detectionRadius * 1.2f)
    {
        LOG_DEBUG << context_.gameObject.GetName() << " lost target " << target->GetName() << " (too far)";
        target = nullptr;

        context_.controller.pushEventToQueue(TargetLostEvent{});
        context_.characterController.pushEventToQueue(EndForwardMoveEvent{});
        return;
    }

    if (distance <= context_.controller.getAttackRadius())
    {
        context_.characterController.pushEventToQueue(EndForwardMoveEvent{});
        context_.controller.pushEventToQueue(TargetInAttackRangeEvent{});
        return;
    }
    auto& currentPath = context_.currentPath;

    pathUpdateTimer_ -= deltaTime;

    if (not context_.navigationManager.Raycast(currentPos, targetPos))
    {
        currentPath.clear();
        moveTo(targetPos);
    }
    else if (pathUpdateTimer_ <= 0.f)
    {
        pathUpdateTimer_ = (distance > 20.f) ? 0.6f : 0.3f;
        currentPath      = context_.navigationManager.CalculatePath(currentPos, targetPos);

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
void AIChaseState::onLeave()
{
    pathUpdateTimer_ = -1.f;
    target           = nullptr;
}
void AIChaseState::moveTo(const vec3& targetPos)
{
    context_.characterController.pushEventToQueue(MoveForwardEvent{});
    const auto currentPos = context_.gameObject.GetWorldTransform().GetPosition();

    auto direction = targetPos - currentPos;
    direction.y    = 0.0f;

    if (glm::length(direction) > 0.0001f)
    {
        auto targetRotation = Utils::calculateTargetRotation(glm::normalize(direction));
        context_.characterController.pushEventToQueue(RotateTargetEvent{targetRotation});
    }
}
}  // namespace Components
}  // namespace GameEngine
