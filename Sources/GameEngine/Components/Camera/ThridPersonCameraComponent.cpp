#include "ThridPersonCameraComponent.h"
#include <Input/InputManager.h>
#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
ComponentsType ThridPersonCameraComponent::type = ComponentsType::ThridPersonCamera;

ThridPersonCameraComponent::ThridPersonCameraComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , zoomSpeed_(0.1f)
{
    auto thridCamera = std::make_unique<ThirdPersonCamera>(componentContext.inputManager_, gameObject.worldTransform);
    auto ptrCam      = thridCamera.get();

    componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::MOUSE_WHEEL, [ptrCam, this]() {ptrCam->CalculateZoom(zoomSpeed_); });
    componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::MOUSE_WHEEL, [ptrCam, this]() {ptrCam->CalculateZoom(-1.f * zoomSpeed_); });
    componentContext_.camera_.Set(std::move(thridCamera));
}

void ThridPersonCameraComponent::ReqisterFunctions()
{
}

}  // namespace Components
}  // namespace GameEngine