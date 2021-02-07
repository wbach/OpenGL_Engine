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
    componentContext_.camera_.Set(*previousCamera_);
}

void ThridPersonCameraComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, [this]() { init(); });
}

void ThridPersonCameraComponent::init()
{
    camera_ = std::make_unique<ThirdPersonCamera>(componentContext_.inputManager_, thisObject_.GetTransform(), offset_);
    auto ptrCam = camera_.get();

    keysSubscriptionsManager_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        KeyCodes::MOUSE_WHEEL, [ptrCam, this]() { ptrCam->CalculateZoom(zoomSpeed_); });
    keysSubscriptionsManager_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        KeyCodes::MOUSE_WHEEL, [ptrCam, this]() { ptrCam->CalculateZoom(-1.f * zoomSpeed_); });

    previousCamera_ = componentContext_.camera_.Get();
    componentContext_.camera_.Set(*camera_);
}

void ThridPersonCameraComponent::registerReadFunctions()
{
    ReadFunctions::instance().componentsReadFunctions.insert(
        {COMPONENT_STR, [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
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
