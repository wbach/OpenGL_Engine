#include "AIController.h"

#include <Logger/Log.h>

#include "CharacterController/CharacterController.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Navigation/NavigationManager.h"

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
AIController::AIController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<AIController>(), componentContext, gameObject)
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
void AIController::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<AIController>(componentContext, gameObject);

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<AIController>(), readFunc);
}
void AIController::write(TreeNode& node) const
{
    BaseComponent::write(node);
}

void AIController::MoveTo(const vec3& targetPosition)
{
    LOG_DEBUG << "Moving " << thisObject_.GetName() << " to " << targetPosition;

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

    LOG_DEBUG << "currentPos: " << currentPos << " Point nextWaypoint: " << nextWaypoint;

    auto toTarget = vec2(nextWaypoint.x - currentPos.x, nextWaypoint.z - currentPos.z);
    auto distance = glm::length(toTarget);

    if (distance < 0.5f)
    {
        LOG_DEBUG << "Point reached. Waypoint : " << nextWaypoint;
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
        isMovingForward_ = true;
    }
}

}  // namespace Components
}  // namespace GameEngine
