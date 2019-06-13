#include "PlayerInputController.h"
#include <algorithm>
#include <ctime>
#include "GameEngine/Components/Animation/Animator.h"
#include "Input/InputManager.h"
#include "Messages/TransformMessages/TransformMsgReq.h"
#include "TestGame/MRpg/MrpgGameContext.h"
#include "UtilsNetwork/Gateway.h"

using namespace common::Controllers::CharacterActions;

namespace AvatarGame
{
PlayerInputController::PlayerInputController(GameEngine::Components::Animator* animator, Input::InputManager* manager, common::Controllers::CharacterController* characterController)
    : inputManager_(manager)
    , animator_(animator)
    , characterController_(characterController)
{
    SubscribeForPushActions();
    SubscribeForPopActions();
}

void PlayerInputController::SubscribeForPushActions()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::W, [&]() {
        AddState(MOVE_FORWARD);
        SetRunAnim();
        animator_->animationSpeed_ = fabs(animator_->animationSpeed_);
    });
    inputManager_->SubscribeOnKeyDown(KeyCodes::S, [&]() {
        AddState(MOVE_BACKWARD);
        SetRunAnim();
        animator_->animationSpeed_ = -1.f * fabs(animator_->animationSpeed_);
    });
    inputManager_->SubscribeOnKeyDown(KeyCodes::A, [&]() { AddState(ROTATE_LEFT); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::D, [&]() { AddState(ROTATE_RIGHT); });
}
void PlayerInputController::SubscribeForPopActions()
{
    inputManager_->SubscribeOnKeyUp(KeyCodes::W, [&]() {
        RemoveState(MOVE_FORWARD);
        SetIdleAnim();
    });
    inputManager_->SubscribeOnKeyUp(KeyCodes::S, [&]() {
        RemoveState(MOVE_BACKWARD);
        SetIdleAnim();
    });
    inputManager_->SubscribeOnKeyUp(KeyCodes::A, [&]() { RemoveState(ROTATE_LEFT); });
    inputManager_->SubscribeOnKeyUp(KeyCodes::D, [&]() { RemoveState(ROTATE_RIGHT); });
}

bool PlayerInputController::FindState(common::Controllers::CharacterActions::Type type)
{
    return std::find(states_.begin(), states_.end(), type) != states_.end();
    ;
}

void PlayerInputController::AddState(common::Controllers::CharacterActions::Type state)
{
    if (FindState(state))
        return;

    characterController_->AddState(state);
    states_.push_back(state);
}

void PlayerInputController::RemoveState(common::Controllers::CharacterActions::Type state)
{
    if (!FindState(state))
        return;

    characterController_->RemoveState(state);
    states_.remove(state);
}

void PlayerInputController::SetRunAnim()
{
    auto anim = animator_->GetCurrentAnimationName();

    if (anim != "Run")
        animator_->ChangeAnimation("Run");
}

void PlayerInputController::SetIdleAnim()
{
    auto anim = animator_->GetCurrentAnimationName();
    if (anim != "Idle")
        animator_->ChangeAnimation("Idle");
}
}  // namespace AvatarGame
