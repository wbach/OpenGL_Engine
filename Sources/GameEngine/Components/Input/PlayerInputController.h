#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Controllers/CharacterController.h"
#include <Common/Controllers/CharacterController/CharacterActions.h>

namespace GameEngine
{
namespace Components
{
class PlayerInputController : public BaseComponent
{
public:
    PlayerInputController(const ComponentContext& componentContext, GameObject& gameObject);

    virtual void ReqisterFunctions() override;

private:
    void Init();
    void SubscribeForPushActions();
    void SubscribeForPopActions();
    bool FindState(CharacterController::Action type);
    void AddState(CharacterController::Action state);
    void RemoveState(CharacterController::Action state);
    void SetRunAnim();
    void SetIdleAnim();

private:
    Animator* animator_;
    CharacterController* characterController_;
    std::vector<CharacterController::Action> states_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
