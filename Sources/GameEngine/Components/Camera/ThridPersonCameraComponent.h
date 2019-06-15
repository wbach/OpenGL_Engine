#pragma once
#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace Components
{
class ThridPersonCameraComponent : public BaseComponent
{
public:
    ThridPersonCameraComponent(const ComponentContext& componentContext, GameObject& gameObject);
    virtual void ReqisterFunctions() override;

private:
    std::unique_ptr<ICamera> camera_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine