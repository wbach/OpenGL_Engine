#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include <Common/Controllers/CharacterController/Character.h>

namespace GameEngine
{
namespace Components
{
class CharacterController : public BaseComponent
{
public:
    CharacterController(const ComponentContext& componentContext, GameObject& gameObject);

    virtual void ReqisterFunctions() override;

    void Update();

    void SetRunSpeed(float);
    void SetTurnSpeed(float);
    void SetJumpPower(float);

    common::Controllers::CharacterController& Get();

private:
    common::Controllers::CharacterController characterController_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
