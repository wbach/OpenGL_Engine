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
AimState::AimState(Context& context, const vec3& rcp)
    : context{context}
    , joint{nullptr}
    , referenceRelativeCamerePosition{rcp, 1.f}
    , relativeCamerePosition{rcp.x, 0.f, rcp.z, 1.f}
    , lookAtLocalPosition(relativeCamerePosition)
    , yTranslation{glm::translate(vec3(0.f, rcp.y, 0.f))}
{
    lookAtLocalPosition.z *= -1.f;
}

void AimState::onEnter(const StartAimEvent& event)
{
    context.camera.setOnUpdate([this]() { cameraUpdate(); });
    cameraUpdate();
    setJointIfNeeded(event);
}

const vec4& AimState::getRelativeCamerePosition() const
{
    return referenceRelativeCamerePosition;
}

const vec4& AimState::getLookAtPosition() const
{
    return lookAtLocalPosition;
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
    parentWorldTransform      = parentWorldTransform * yTranslation * glm::mat4_cast(jointMatrix);

    auto worldCameraPosition = parentWorldTransform * relativeCamerePosition;
    context.camera.SetPosition(worldCameraPosition);

    auto lookAtPosition = parentWorldTransform * lookAtLocalPosition;
    context.camera.LookAt(lookAtPosition);
}

void AimState::setJointIfNeeded(const StartAimEvent& event)
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
