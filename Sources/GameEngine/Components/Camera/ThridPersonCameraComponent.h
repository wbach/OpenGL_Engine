#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include <Input/KeysSubscriptionsManager.h>

namespace GameEngine
{
struct ICamera;

namespace Components
{
class ThridPersonCameraComponent : public BaseComponent
{
public:
    ThridPersonCameraComponent(ComponentContext& componentContext, GameObject& gameObject);
    void CleanUp() override;
    void ReqisterFunctions() override;

private:
    Input::KeysSubscriptionsManager keysSubscriptionsManager_;
    float zoomSpeed_;
    std::unique_ptr<ICamera> camera_;
    vec3 offset_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
