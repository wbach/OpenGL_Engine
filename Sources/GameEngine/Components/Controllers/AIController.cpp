#include "AIController.h"

#include <Logger/Log.h>

#include "CharacterController/CharacterController.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Navigation/NavigationManager.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace Components
{
namespace
{
Quaternion calculateTargetRotation(const vec3& direction)
{
    float angle = atan2f(direction.x, direction.z);
    return glm::angleAxis(angle, vec3(0.0f, 1.0f, 0.0f));
}
}  // namespace

REGISTER_COMPONENT(AIController)

AIController::AIController(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
    , characterController_{nullptr}
{
}
void AIController::CleanUp()
{
}
void AIController::ReqisterFunctions()
{
    RegisterFunction(
        FunctionType::OnStart, [this]() { Init(); }, MakeDependencies<CharacterController>());
    RegisterFunction(FunctionType::Update, [this]() { Update(); });
}
void AIController::Reload()
{
}
void AIController::Init()
{
    characterController_ = thisObject_.GetComponent<CharacterController>();
}
void AIController::Update()
{
    if (currentPath_.empty())
        return;

    UpdateNavigation();
}

void AIController::read(const TreeNode&)
{
}

void AIController::write(TreeNode& node) const
{
}

void AIController::MoveTo(const vec3& targetPosition, MoveType type)
{
    LOG_DEBUG << "Moving(" << magic_enum::enum_name(type) << ") " << thisObject_.GetName() << " to " << targetPosition;

    moveType_        = type;
    auto& navManager = componentContext_.navigationManager;
    vec3 startPos    = thisObject_.GetWorldTransform().GetPosition();

    currentPath_ = navManager.CalculatePath(startPos, targetPosition);

    if (currentPath_.empty())
    {
        LOG_WARN << "AIController: Could not find path to target!";
    }
}
void AIController::UpdateNavigation()
{
    auto currentPos   = thisObject_.GetWorldTransform().GetPosition();
    auto nextWaypoint = currentPath_.front();

    auto toTarget = vec2(nextWaypoint.x - currentPos.x, nextWaypoint.z - currentPos.z);
    auto distance = glm::length(toTarget);

    if (distance < 0.5f)
    {
        currentPath_.erase(currentPath_.begin());

        if (currentPath_.empty())
        {
            characterController_->pushEventToQueue(EndForwardMoveEvent{});
            isMovingForward_ = false;
            return;
        }
    }

    auto direction = nextWaypoint - currentPos;
    direction.y    = 0.0f;
    if (glm::length(direction) > 0.0001f)
    {
        auto targetRotation = calculateTargetRotation(glm::normalize(direction));
        characterController_->pushEventToQueue(RotateTargetEvent{targetRotation});
    }

    if (not isMovingForward_)
    {
        characterController_->pushEventToQueue(MoveForwardEvent{});
        if (moveType_ == MoveType::WALK)
        {
            characterController_->pushEventToQueue(WalkChangeStateEvent{});
        }
        else if (moveType_ == MoveType::SPRINT)
        {
            characterController_->pushEventToQueue(SprintStateChangeEvent{});
        }
        isMovingForward_ = true;
    }
}

}  // namespace Components
}  // namespace GameEngine
