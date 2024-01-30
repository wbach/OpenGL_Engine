#include "TransitionState.h"

#include "AimState.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
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
{
}
void TransitionState::onEnter(RotateableRunState& prevState, const StartAimEvent& event)
{
    if (thridPersonCameraComponent)
        thridPersonCameraComponent->handleEvent(event);
}
void TransitionState::onEnter(AimState&, const StopAimEvent& event)
{
    if (thridPersonCameraComponent)
        thridPersonCameraComponent->handleEvent(event);
}
void TransitionState::cameraUpdate()
{
}

}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
