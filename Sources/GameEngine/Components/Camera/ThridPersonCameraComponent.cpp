#include "ThridPersonCameraComponent.h"
#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
ComponentsType ThridPersonCameraComponent::type = ComponentsType::Camera;

ThridPersonCameraComponent::ThridPersonCameraComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
{
    camera_ = std::make_unique<ThirdPersonCamera>(componentContext.inputManager_, gameObject.worldTransform);
    //componentContext_.camera_ = camera_;
}

void ThridPersonCameraComponent::ReqisterFunctions()
{

}

}  // namespace Components
}  // namespace GameEngine