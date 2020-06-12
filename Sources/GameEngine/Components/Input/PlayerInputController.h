#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Controllers/CharacterController.h"
#include <Common/Controllers/CharacterController/CharacterActions.h>
#include <Input/KeysSubscriptionsManager.h>

namespace GameEngine
{
namespace Components
{
class PlayerInputController : public BaseComponent
{
public:
    PlayerInputController(ComponentContext& componentContext, GameObject& gameObject);

    void CleanUp() override;
    void ReqisterFunctions() override;

public:
    std::string idleAnimationName_;
    std::string walkAnimationName_;
    std::string runAnimationName_;

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
    Input::KeysSubscriptionsManager subscriptions_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
