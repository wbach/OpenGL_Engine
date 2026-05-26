#include "AIStateBase.h"

#include <Utils/GLM/GLMUtils.h>

#include "../AIController.h"
#include "../AIControllerContext.h"
#include "GameEngine/Components/Controllers/AI/AIControllerEvents.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Navigation/NavigationManager.h"

namespace GameEngine
{
namespace Components
{

AIStateBase::AIStateBase(AIControllerContext& context)
    : context_{context}
{
}
void AIStateBase::onEnter()
{
    context_.currentPath.clear();
    isMovingForward_ = false;
}
void AIStateBase::onLeave()
{
    context_.currentPath.clear();
    isMovingForward_ = false;
    context_.characterController.pushEventToQueue(EndForwardMoveEvent{});
}
AIStateBase::NavigationStatus AIStateBase::updateNavigation()
{
    if (context_.currentPath.empty())
    {
        return NavigationStatus::Failed;
    }

    auto currentPos   = context_.gameObject.GetWorldTransform().GetPosition();
    auto nextWaypoint = context_.currentPath.front();

    auto toTarget = vec2(nextWaypoint.x - currentPos.x, nextWaypoint.z - currentPos.z);
    auto distance = glm::length(toTarget);

    if (distance < 0.5f)
    {
        context_.currentPath.erase(context_.currentPath.begin());

        if (context_.currentPath.empty())
        {
            return NavigationStatus::Done;
        }
    }

    auto direction = nextWaypoint - currentPos;
    direction.y    = 0.0f;
    if (glm::length(direction) > 0.0001f)
    {
        auto targetRotation = Utils::calculateTargetRotation(glm::normalize(direction));
        context_.characterController.pushEventToQueue(RotateTargetEvent{targetRotation});
    }

    if (not isMovingForward_)
    {
        context_.characterController.pushEventToQueue(MoveForwardEvent{});
        if (moveType_ == AIMoveType::WALK)
        {
            context_.characterController.pushEventToQueue(WalkChangeStateEvent{});
        }
        else if (moveType_ == AIMoveType::SPRINT)
        {
            context_.characterController.pushEventToQueue(SprintStateChangeEvent{});
        }
        isMovingForward_ = true;
    }

    return NavigationStatus::InProgress;
}
void AIStateBase::startMoveTo(const vec3& targetPosition, AIMoveType moveType)
{
    moveType_        = moveType;
    auto& navManager = context_.navigationManager;
    vec3 startPos    = context_.gameObject.GetWorldTransform().GetPosition();

    context_.currentPath = navManager.CalculatePath(context_.gameObject, startPos, targetPosition);

    if (context_.currentPath.empty())
    {
        LOG_WARN << context_.gameObject.GetName()
                 << ": AIController, Could not find path to target! targetPosition : " << targetPosition;
    }
}
}  // namespace Components
}  // namespace GameEngine
