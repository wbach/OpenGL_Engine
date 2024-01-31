#include "TransitionState.h"

#include "AimState.h"
#include "GameEngine/Camera/CustomCamera.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "RotateableRunState.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
TransitionState::TransitionState(Context& context)
    : context{context}
    , thridPersonCameraComponent{context.gameObject.GetComponent<ThridPersonCameraComponent>()}
    , progress{0}
    , transitionLength{0.1f}
{
}

void TransitionState::onEnter()
{
    progress = 0;
    context.camera.setOnUpdate([this]() { cameraUpdate(); });
}
void TransitionState::onEnter(RotateableRunState& rotateableRunState, const StartAimEvent& event)
{
    if (thridPersonCameraComponent)
    {
        auto aimState = std::get<AimState>(thridPersonCameraComponent->fsm->states);

        sourcePosition = rotateableRunState.getRelativeCamerePosition();
        targetPosition = aimState.getRelativeCamerePosition();
        calculateLookAts();
    }

    processingEvent = event;
}
void TransitionState::onEnter(AimState& aimState, const StopAimEvent& event)
{
    if (thridPersonCameraComponent)
    {
        auto rotateableRunState = std::get<RotateableRunState>(thridPersonCameraComponent->fsm->states);

        sourcePosition = aimState.getRelativeCamerePosition();
        targetPosition = rotateableRunState.getRelativeCamerePosition();
        calculateLookAts();
    }

    processingEvent = event;
}
void TransitionState::cameraUpdate()
{
    progress += context.displayManager.GetTime().deltaTime;
    auto t = progress / transitionLength;

    if (t > 1.0f)
    {
        if (thridPersonCameraComponent)
            thridPersonCameraComponent->pushEventToQueue(processingEvent);
        return;
    }

    auto relativeCamerePosition = glm::mix(sourcePosition, targetPosition, t);
    auto lookAtLocalPosition    = glm::mix(sourceLookAt, targetLookAt, t);

    auto worldCameraPosition = context.gameObject.GetWorldTransform().GetMatrix() * relativeCamerePosition;
    context.camera.SetPosition(worldCameraPosition);

    auto lookAtPosition = context.gameObject.GetWorldTransform().GetMatrix() * lookAtLocalPosition;
    context.camera.LookAt(lookAtPosition);
}

void TransitionState::calculateLookAts()
{
    sourceLookAt = sourcePosition;
    sourceLookAt.z *= -1.f;

    targetLookAt = targetPosition;
    targetLookAt.z *= -1.f;
}

}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
