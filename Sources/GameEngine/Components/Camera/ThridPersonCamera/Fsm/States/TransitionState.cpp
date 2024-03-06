#include "TransitionState.h"

#include "AimState.h"
#include "GameEngine/Camera/CustomCamera.h"
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
    StateBase::setUpdateFunc();
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
    if (sourcePosition and targetPosition)
        relativeCamerePosition = glm::mix(*sourcePosition, *targetPosition, smoothProgress);

    StateBase::cameraUpdate();
}

}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
