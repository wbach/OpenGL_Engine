#include "ThridPersonCameraComponent.h"
#include <Input/InputManager.h>
#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
ComponentsType ThridPersonCameraComponent::type = ComponentsType::ThridPersonCamera;

ThridPersonCameraComponent::ThridPersonCameraComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , keysSubscriptionsManager_(componentContext.inputManager_)
    , zoomSpeed_(0.1f)
    , offset_(0, 1.8f, 0)
{
    camera_ = std::make_unique<ThirdPersonCamera>(componentContext.inputManager_, gameObject.GetTransform(), offset_);
    auto ptrCam = camera_.get();

    keysSubscriptionsManager_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        KeyCodes::MOUSE_WHEEL, [ptrCam, this]() { ptrCam->CalculateZoom(zoomSpeed_); });
    keysSubscriptionsManager_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        KeyCodes::MOUSE_WHEEL, [ptrCam, this]() { ptrCam->CalculateZoom(-1.f * zoomSpeed_); });
    componentContext_.camera_.Set(*camera_);
}

void ThridPersonCameraComponent::CleanUp()
{
    keysSubscriptionsManager_.UnsubscribeKeys();
}

void ThridPersonCameraComponent::ReqisterFunctions()
{
}

}  // namespace Components
}  // namespace GameEngine
