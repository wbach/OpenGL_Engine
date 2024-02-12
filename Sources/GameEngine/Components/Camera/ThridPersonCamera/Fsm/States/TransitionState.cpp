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
    , transitionLength{0.5f}
{
}

void TransitionState::onEnter()
{
    progress = 0;
}
void TransitionState::onEnter(const StartAimEvent& event)
{
    if (thridPersonCameraComponent)
    {
        auto followingState = std::get<FollowingState>(thridPersonCameraComponent->fsm->states);
        auto aimState       = std::get<AimState>(thridPersonCameraComponent->fsm->states);

        sourcePosition = followingState.getRelativeCamerePosition();
        targetPosition = aimState.getRelativeCamerePosition();
        calculateLookAts();
    }

    transitionLength = event.transitionLength;
    processingEvent  = event;
    context.camera.setOnUpdate([this]() { cameraUpdate(); });
}
void TransitionState::onEnter(const StopAimEvent& event)
{
    if (thridPersonCameraComponent)
    {
        auto followingState = std::get<FollowingState>(thridPersonCameraComponent->fsm->states);
        auto aimState       = std::get<AimState>(thridPersonCameraComponent->fsm->states);

        sourcePosition = aimState.getRelativeCamerePosition();
        targetPosition = followingState.getRelativeCamerePosition();
        calculateLookAts();
    }
    transitionLength = event.transitionLength;
    processingEvent  = event;
    context.camera.setOnUpdate([this]() { cameraUpdate(); });
}

void TransitionState::onEnter(const MouseInactivityEvent& event)
{
    if (thridPersonCameraComponent)
    {
        auto rotateableRunState = std::get<RotateableRunState>(thridPersonCameraComponent->fsm->states);
        auto followingState     = std::get<FollowingState>(thridPersonCameraComponent->fsm->states);

        sourcePosition = rotateableRunState.getRelativeCamerePosition();
        targetPosition = followingState.getRelativeCamerePosition();
        calculateLookAts();
    }

    // progress        = 2.f;
    transitionLength = event.transitionLength;
    processingEvent  = event;
    context.camera.setOnUpdate([this, event]() { cameraUpdateWithAditionalRotation(event); });
}

bool TransitionState::transitionCondition(const StopAimEvent& event)
{
    if (progress > 1.f)
        return true;

    progress = 1.f - progress;
    onEnter(event);
    return false;
}

bool TransitionState::transitionCondition(const StartAimEvent& event)
{
    if (progress > 1.f)
        return true;

    progress = 1.f - progress;
    onEnter(event);
    return false;
}
void TransitionState::cameraUpdate()
{
    progress += context.displayManager.GetTime().deltaTime / transitionLength;

    if (progress > 1.0f)
    {
        if (thridPersonCameraComponent)
            thridPersonCameraComponent->pushEventToQueue(processingEvent);
        return;
    }

    float smoothProgress = glm::smoothstep(0.f, 1.f, progress);
    auto relativeCamerePosition = glm::mix(sourcePosition, targetPosition, smoothProgress);
    auto lookAtLocalPosition    = glm::mix(sourceLookAt, targetLookAt, smoothProgress);

    auto worldCameraPosition = context.gameObject.GetWorldTransform().GetMatrix() * relativeCamerePosition;
    context.camera.SetPosition(worldCameraPosition);

    auto lookAtPosition = context.gameObject.GetWorldTransform().GetMatrix() * lookAtLocalPosition;
    context.camera.LookAt(lookAtPosition);
}

void TransitionState::cameraUpdateWithAditionalRotation(const MouseInactivityEvent& event)
{
    progress += context.displayManager.GetTime().deltaTime / transitionLength;

    if (progress > 1.0f)
    {
        if (thridPersonCameraComponent)
            thridPersonCameraComponent->pushEventToQueue(processingEvent);
        return;
    }

    float smoothProgress = glm::smoothstep(0.f, 1.f, progress);

    auto pitch = glm::mix(event.pitch, 0.f, smoothProgress);
    auto yaw   = glm::mix(event.yaw, 0.f, smoothProgress);

    auto rotX = glm::normalize(glm::angleAxis(glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f)));
    auto rotY = glm::normalize(glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f)));

    auto parentWorldTransform = context.gameObject.GetWorldTransform().GetMatrix();
    parentWorldTransform      = parentWorldTransform * event.yTranslation * mat4_cast(rotY * rotX);

    auto worldCameraPosition = parentWorldTransform * event.relativeCamerePosition;
    context.camera.SetPosition(worldCameraPosition);

    auto lookAtPosition = parentWorldTransform * event.lookAtLocalPosition;
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
