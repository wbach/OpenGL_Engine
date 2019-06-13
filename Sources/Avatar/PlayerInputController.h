#pragma once
#include <memory>
#include "Common/Controllers/CharacterController/Character.h"
#include "Common/Hero/Stats.h"
#include "Components/Animation/Animator.h"

namespace Input
{
class InputManager;
}

namespace AvatarGame
{
class PlayerInputController
{
public:
    PlayerInputController(GameEngine::Components::Animator* animator, Input::InputManager* manager, common::Controllers::CharacterController* characterController);

private:
    void SubscribeForPushActions();
    void SubscribeForPopActions();
    bool FindState(common::Controllers::CharacterActions::Type type);
    void AddState(common::Controllers::CharacterActions::Type state);
    void RemoveState(common::Controllers::CharacterActions::Type state);
    void SetRunAnim();
    void SetIdleAnim();

private:
    Input::InputManager* inputManager_;
    GameEngine::Components::Animator* animator_;
    common::Controllers::CharacterController* characterController_;
    std::list<common::Controllers::CharacterActions::Type> states_;
};

typedef std::unique_ptr<PlayerInputController> PlayerControllerPtr;
}  // namespace AvatarGame
