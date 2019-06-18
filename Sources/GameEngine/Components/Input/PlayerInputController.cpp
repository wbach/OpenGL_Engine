#include "PlayerInputController.h"
#include <Input/InputManager.h>
#include "GameEngine/Objects/GameObject.h"

using namespace common::Controllers;

namespace GameEngine
{
namespace Components
{
ComponentsType PlayerInputController::type = ComponentsType::PlayerInputController;

PlayerInputController::PlayerInputController(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , animator_{nullptr}
    , characterController_{nullptr}
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
        AddState(CharacterActions::MOVE_FORWARD);
        SetRunAnim();
        animator_->animationSpeed_ = fabsf(animator_->animationSpeed_);
    });
    componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::S, [&]() {
        AddState(CharacterActions::MOVE_BACKWARD);
        SetRunAnim();
        animator_->animationSpeed_ = -1.f * fabsf(animator_->animationSpeed_);
    });
    componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::A, [&]() { AddState(CharacterActions::ROTATE_LEFT); });
    componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::D,
                                                       [&]() { AddState(CharacterActions::ROTATE_RIGHT); });
}

void PlayerInputController::SubscribeForPopActions()
{
    componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::W, [&]() {
        RemoveState(CharacterActions::MOVE_FORWARD);
        SetIdleAnim();
    });
    componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::S, [&]() {
        RemoveState(CharacterActions::MOVE_BACKWARD);
        SetIdleAnim();
    });
    componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::A,
                                                     [&]() { RemoveState(CharacterActions::ROTATE_LEFT); });
    componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::D,
                                                     [&]() { RemoveState(CharacterActions::ROTATE_RIGHT); });
}

bool PlayerInputController::FindState(common::Controllers::CharacterActions::Type type)
{
    return std::find(states_.begin(), states_.end(), type) != states_.end();
}

void PlayerInputController::AddState(common::Controllers::CharacterActions::Type state)
{
    if (FindState(state) or not characterController_)
        return;

    characterController_->Get().AddState(state);
    states_.push_back(state);
}

void PlayerInputController::RemoveState(common::Controllers::CharacterActions::Type state)
{
    if (not FindState(state) or not characterController_)
        return;

    characterController_->Get().RemoveState(state);
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
}  // namespace Components
}  // namespace GameEngine
