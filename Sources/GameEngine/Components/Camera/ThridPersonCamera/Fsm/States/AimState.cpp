#include "AimState.h"

#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Camera/CustomCamera.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Objects/GameObject.h"
#include "Input/InputManager.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
AimState::AimState(Context& context)
    : context{context}
    , relativeCamerePosition(-0.25f, 0.f, -0.75f, 1.f)
    , joint{nullptr}

{
}

void AimState::onEnter(const StartAimEvent& event)
{
    context.inputManager.ShowCursor(true);
    context.inputManager.SetReleativeMouseMode(false);
    context.camera.setOnUpdate([this]() { cameraUpdate(); });
    cameraUpdate();
    context.inputManager.SetReleativeMouseMode(true);
    setJointIfNeeded(event);
}

void AimState::cameraUpdate()
{
    if (not joint)
    {
        ERROR_LOG("Joint not set !");
        return;
    }

    auto jointMatrix          = joint->additionalRotations.y.value_ * joint->additionalRotations.x.value_;
    auto parentWorldTransform = context.gameObject.GetWorldTransform().GetMatrix();
    parentWorldTransform      = parentWorldTransform * glm::translate(vec3(0.f, 1.f, 0)) * glm::mat4_cast(jointMatrix);

    auto worldCameraPosition = parentWorldTransform * relativeCamerePosition;
    context.camera.SetPosition(worldCameraPosition);

    auto lookAtLocalPosition = relativeCamerePosition;
    lookAtLocalPosition.z *= -1.f;
    auto lookAtPosition = parentWorldTransform * lookAtLocalPosition;
    context.camera.LookAt(lookAtPosition);
}

void AimState::setJointIfNeeded(const StartAimEvent &event)
{
    if (joint and joint->id == event.jointId)
        return;

    auto animator = context.gameObject.GetComponent<Animator>();
    if (animator)
    {
        joint = animator->GetJoint(event.jointId);
    }

    if (not joint)
    {
        ERROR_LOG("Joint : " + context.jointName + " not found");
    }
}

}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
