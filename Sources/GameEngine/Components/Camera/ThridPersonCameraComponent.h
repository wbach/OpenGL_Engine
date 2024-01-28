#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include <Input/KeysSubscriptionsManager.h>

namespace GameEngine
{
struct ICamera;
class ThirdPersonCamera;

namespace Components
{
class ThridPersonCameraComponent : public BaseComponent
{
public:
    ThridPersonCameraComponent(ComponentContext& componentContext, GameObject& gameObject);
    void CleanUp() override;
    void ReqisterFunctions() override;
    ThirdPersonCamera* thirdPersonCamera;

private:
    void init();

private:
    Input::KeysSubscriptionsManager keysSubscriptionsManager_;
    std::optional<IdType> cameraId_;
    float zoomSpeed_;
    vec3 offset_;


public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
