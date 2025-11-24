#include "TransitionState.h"

#include <variant>

#include "AimState.h"
#include "GameEngine/Camera/CustomCamera.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/Fsm/ThridPersonCameraEvents.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "Logger/Log.h"
#include "RotateableRunState.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
TransitionState::TransitionState(Context& context)
    : StateBase(context)
    , progress{0}
    , transitionLength{2.5f}
{
}

void TransitionState::onEnter()
{
    progress = 0;
    sourcePosition.reset();
    targetPosition.reset();
    StateBase::onEnter();
}
void TransitionState::onEnter(const StartAimEvent& event)
{
    sourcePosition = vec4(context.cameraPositions.run, 1.f);
    targetPosition = vec4(context.cameraPositions.aim, 1.f);

    transitionLength = event.transitionLength;
    processingEvent  = event;
    pitchConversion  = 1.f;
}
void TransitionState::onEnter(const StopAimEvent& event)
{
    sourcePosition = vec4(context.cameraPositions.aim, 1.f);
    targetPosition = vec4(context.cameraPositions.run, 1.f);

    transitionLength = event.transitionLength;
    processingEvent  = event;
    pitchConversion  = -1.f;
}

void TransitionState::onEnter(const MouseInactivityEvent& event)
{
    relativeCamerePosition = context.cameraPositions.run;

    pitchStart = context.pitch;
    yawStart   = context.yaw;

    transitionLength = event.transitionLength;
    processingEvent  = event;
    pitchConversion  = 1.f;
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
void TransitionState::update()
{
    progress += context.displayManager.GetTime().deltaTime / transitionLength;

    if (progress > 1.0f)
    {
        pushEventToQueue(processingEvent);
        return;
    }

    float smoothProgress = glm::smoothstep(0.f, 1.f, progress);

    if (std::holds_alternative<StartAimEvent>(processingEvent) or std::holds_alternative<StopAimEvent>(processingEvent))
    {
        if (sourcePosition and targetPosition)
        {
            relativeCamerePosition = glm::mix(*sourcePosition, *targetPosition, smoothProgress);
            StateBase::cameraUpdate();
        }
        return;
    }

    context.pitch = glm::mix(pitchStart, 0.f, smoothProgress);
    context.yaw   = glm::mix(yawStart, 0.f, smoothProgress);

    StateBase::cameraUpdate();
}
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
