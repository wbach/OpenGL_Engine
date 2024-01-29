#include "AimState.h"

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
    , relativeCamerePosition(-0.25f, 1.0f, -0.75f, 1.f)
    , joint{nullptr}

{
    auto animator = context.gameObject.GetComponent<Animator>();
    if (animator)
    {
        joint = animator->GetJoint("mixamorig:Spine1");
    }

    if (not joint)
    {
        ERROR_LOG("Joint not found");
    }
}

void AimState::onEnter(const StartAimEvent&)
{
    DEBUG_LOG("StartAimEvent");
    context.inputManager.ShowCursor(true);
    context.inputManager.SetReleativeMouseMode(false);
    context.camera.setOnUpdate([this]() { cameraUpdate(); });
    cameraUpdate();
    context.inputManager.SetReleativeMouseMode(true);
}

void AimState::cameraUpdate()
{
    // DEBUG_LOG("AimState::cameraUpdate()");
//    auto [pos, rot, scale] = Utils::decompose(joint->additionalUserMofiyTransform);
//    Rotation r(rot);
//    auto erot = r.GetEulerDegrees();
//    Rotation nr(DegreesVec3(erot.value.z, erot.value.y, erot.value.x));

    auto parentWorldTransform = context.gameObject.GetWorldTransform().GetMatrix();
    parentWorldTransform = parentWorldTransform ;//* joint->additionalUserMofiyTransform;

    auto worldCameraPosition = parentWorldTransform * relativeCamerePosition;
    context.camera.SetPosition(worldCameraPosition);

    auto lookAtLocalPosition = relativeCamerePosition;
    lookAtLocalPosition.z *= -1.f;
    auto lookAtPosition = parentWorldTransform * lookAtLocalPosition;
    context.camera.LookAt(lookAtPosition);
}

}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
