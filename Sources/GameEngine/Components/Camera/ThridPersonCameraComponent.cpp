#include "ThridPersonCameraComponent.h"

#include <Input/InputManager.h>

#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string& COMPONENT_STR{"ThridPersonCamera"};
}
ThridPersonCameraComponent::ThridPersonCameraComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(ThridPersonCameraComponent).hash_code(), componentContext, gameObject)
    , keysSubscriptionsManager_(componentContext.inputManager_)
    , zoomSpeed_(0.1f)
    , offset_(0, 1.8f, 0)
{
}

void ThridPersonCameraComponent::CleanUp()
{
    keysSubscriptionsManager_.UnsubscribeKeys();

    if (cameraId_)
    {
        componentContext_.camera_.remove(*cameraId_);
    }
}

void ThridPersonCameraComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, [this]() { init(); });
}

void ThridPersonCameraComponent::init()
{
    auto camera =
        std::make_unique<ThirdPersonCamera>(componentContext_.inputManager_, thisObject_.GetTransform(), offset_);

    keysSubscriptionsManager_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        KeyCodes::MOUSE_WHEEL, [ptrCam = camera.get(), this]() { ptrCam->CalculateZoom(zoomSpeed_); });
    keysSubscriptionsManager_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        KeyCodes::MOUSE_WHEEL, [ptrCam = camera.get(), this]() { ptrCam->CalculateZoom(-1.f * zoomSpeed_); });

    cameraId_ = componentContext_.camera_.addAndSet(std::move(camera));
}

void ThridPersonCameraComponent::registerReadFunctions()
{
    ReadFunctions::instance().componentsReadFunctions.insert(
        {COMPONENT_STR, [](ComponentContext& componentContext, const TreeNode&, GameObject& gameObject) {
             return std::make_unique<ThridPersonCameraComponent>(componentContext, gameObject);
         }});
}

void ThridPersonCameraComponent::write(TreeNode& node) const
{
    const std::string CSTR_TYPE = "type";
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
}

}  // namespace Components
}  // namespace GameEngine
