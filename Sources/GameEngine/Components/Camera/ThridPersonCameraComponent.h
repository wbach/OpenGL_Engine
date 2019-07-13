#pragma once
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
struct ICamera;

namespace Components
{
class ThridPersonCameraComponent : public BaseComponent
{
public:
    ThridPersonCameraComponent(const ComponentContext& componentContext, GameObject& gameObject);
    virtual void ReqisterFunctions() override;

private:
    float zoomSpeed_;
    std::unique_ptr<ICamera> camera_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine