#pragma once
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

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine