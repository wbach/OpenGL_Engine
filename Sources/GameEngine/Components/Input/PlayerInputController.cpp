#include "PlayerInputController.h"
#include <Input/InputManager.h>
#include "GameEngine/Objects/GameObject.h"

using namespace common::Controllers;

namespace GameEngine
{
namespace Components
{
ComponentsType PlayerInputController::type = ComponentsType::PlayerInputController;

PlayerInputController::PlayerInputController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , animator_{nullptr}
    , characterController_{nullptr}
{
}

void PlayerInputController::CleanUp()
{

}

void PlayerInputController::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&PlayerInputController::Init, this));
}

void PlayerInputController::Init()
{
    animator_            = thisObject_.GetComponent<Animator>();
    characterController_ = thisObject_.GetComponent<CharacterController>();

    SubscribeForPushActions();
    SubscribeForPopActions();
}

void PlayerInputController::SubscribeForPushActions()
{
    componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::W, [&]() {
        AddState(CharacterController::Action::MOVE_FORWARD);
        SetRunAnim();
        animator_->animationSpeed_ = fabsf(animator_->animationSpeed_);
    });
    componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::S, [&]() {
        AddState(CharacterController::Action::MOVE_BACKWARD);
        SetRunAnim();
        animator_->animationSpeed_ = -1.f * fabsf(animator_->animationSpeed_);
    });
    componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::A, [&]() { AddState(CharacterController::Action::ROTATE_LEFT); });
    componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::D,
                                                       [&]() { AddState(CharacterController::Action::ROTATE_RIGHT); });
}

void PlayerInputController::SubscribeForPopActions()
{
    componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::W, [&]() {
        RemoveState(CharacterController::Action::MOVE_FORWARD);
        SetIdleAnim();
    });
    componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::S, [&]() {
        RemoveState(CharacterController::Action::MOVE_BACKWARD);
        SetIdleAnim();
    });
    componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::A,
                                                     [&]() { RemoveState(CharacterController::Action::ROTATE_LEFT); });
    componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::D,
                                                     [&]() { RemoveState(CharacterController::Action::ROTATE_RIGHT); });
}

bool PlayerInputController::FindState(CharacterController::Action type)
{
    return std::find(states_.begin(), states_.end(), type) != states_.end();
}

void PlayerInputController::AddState(CharacterController::Action state)
{
    if (FindState(state) or not characterController_)
        return;

    characterController_->AddState(state);
    states_.push_back(state);
}

void PlayerInputController::RemoveState(CharacterController::Action state)
{
    if (not characterController_)
        return;

    auto iter = std::find(states_.begin(), states_.end(), state);

    if (iter != states_.end())
    {
        characterController_->RemoveState(state);
        states_.erase(iter);
    }
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
}  // namespace Components
}  // namespace GameEngine
