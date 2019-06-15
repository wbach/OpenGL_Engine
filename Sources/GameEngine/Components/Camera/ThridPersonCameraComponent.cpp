#include "ThridPersonCameraComponent.h"
#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
ComponentsType ThridPersonCameraComponent::type = ComponentsType::ThridPersonCamera;

ThridPersonCameraComponent::ThridPersonCameraComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
{
    componentContext_.camera_.Set(
        std::make_unique<ThirdPersonCamera>(componentContext.inputManager_, gameObject.worldTransform));
}

void ThridPersonCameraComponent::ReqisterFunctions()
{
}

}  // namespace Components
}  // namespace GameEngine