#include "PlayerInputController.h"

#include <Input/InputManager.h>

#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Objects/GameObject.h"

using namespace common::Controllers;

namespace GameEngine
{
namespace Components
{
ComponentsType PlayerInputController::type = ComponentsType::PlayerInputController;

PlayerInputController::PlayerInputController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , idleAnimationName_{"Idle"}
    , walkAnimationName_{"Walk"}
    , runAnimationName_{"Run"}
    , jumpAnimationName_{"Jump"}
    , animator_{nullptr}
    , characterController_{nullptr}
    , subscriptions_{componentContext.inputManager_}
{
}

void PlayerInputController::CleanUp()
{
    if (characterController_)
    {
        characterController_->SetJumpCallback(nullptr);
    }

    if (animator_ and connectedBone_)
    {
        animator_->disconnectObjectFromBone(*connectedBone_);
    }
}

void PlayerInputController::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&PlayerInputController::Init, this));
}

void PlayerInputController::Init()
{
    animator_            = thisObject_.GetComponent<Animator>();
    characterController_ = thisObject_.GetComponent<CharacterController>();

    if (animator_ and characterController_)
    {
        if (not weaponChildObjectName_.empty() and not weaponBoneName_.empty())
        {
            auto weaponObject = thisObject_.GetChild(weaponChildObjectName_);
            if (weaponObject)
            {
                connectedBone_ =
                    animator_->connectBoneWithObject(weaponBoneName_, *weaponObject, weponBonePositionOffset_,
                                                     Rotation(DegreesVec3(weponBoneRotationOffsetDegreesEulers_)));
            }
        }

        characterController_->SetJumpCallback([&]() {
            auto moveState = std::find_if(states_.begin(), states_.end(), [](const auto& state) {
                return state == CharacterController::Action::MOVE_FORWARD or
                       state == CharacterController::Action::MOVE_BACKWARD;
            });
            if (moveState == states_.end())
            {
                animator_->ChangeAnimation(idleAnimationName_, Animator::AnimationChangeType::smooth);
            }
            else
            {
                if ((*moveState) == CharacterController::Action::MOVE_FORWARD)
                {
                    animator_->ChangeAnimation(runAnimationName_, Animator::AnimationChangeType::smooth,
                                               Animator::PlayDirection::forward);
                }
                else
                {
                    animator_->ChangeAnimation(runAnimationName_, Animator::AnimationChangeType::smooth,
                                               Animator::PlayDirection::backward);
                }
            }

            RemoveState(CharacterController::Action::JUMP);
        });

        SubscribeForPushActions();
        SubscribeForPopActions();

        auto iter = animator_->animationClips_.find(jumpAnimationName_);
        if (iter != animator_->animationClips_.end())
        {
            iter->second.playType = Animation::AnimationClip::PlayType::once;
        }
    }
}

void PlayerInputController::SubscribeForPushActions()
{
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::W, [&]() {
        AddState(CharacterController::Action::MOVE_FORWARD);
        RemoveState(CharacterController::Action::MOVE_BACKWARD);

        auto jumpState = std::find_if(states_.begin(), states_.end(),
                                      [](const auto& state) { return state == CharacterController::Action::JUMP; });

        if (jumpState == states_.end())
        {
            animator_->ChangeAnimation(runAnimationName_, Animator::AnimationChangeType::smooth,
                                       Animator::PlayDirection::forward);
        }
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::S, [&]() {
        AddState(CharacterController::Action::MOVE_BACKWARD);
        RemoveState(CharacterController::Action::MOVE_FORWARD);
        auto jumpState = std::find_if(states_.begin(), states_.end(),
                                      [](const auto& state) { return state == CharacterController::Action::JUMP; });

        if (jumpState == states_.end())
        {
            animator_->ChangeAnimation(runAnimationName_, Animator::AnimationChangeType::smooth,
                                       Animator::PlayDirection::backward);
        }
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::A, [&]() {
        AddState(CharacterController::Action::ROTATE_LEFT);
        RemoveState(CharacterController::Action::ROTATE_RIGHT);
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::D, [&]() {
        AddState(CharacterController::Action::ROTATE_RIGHT);
        RemoveState(CharacterController::Action::ROTATE_LEFT);
    });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::SPACE, [&]() {
        AddState(CharacterController::Action::JUMP);
        animator_->ChangeAnimation(jumpAnimationName_, Animator::AnimationChangeType::smooth);
    });
}

void PlayerInputController::SubscribeForPopActions()
{
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::W, [&]() {
        RemoveState(CharacterController::Action::MOVE_FORWARD);
        auto jumpState = std::find_if(states_.begin(), states_.end(),
                                      [](const auto& state) { return state == CharacterController::Action::JUMP; });

        if (jumpState == states_.end())
        {
            animator_->ChangeAnimation(idleAnimationName_, Animator::AnimationChangeType::smooth);
        }
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::S, [&]() {
        RemoveState(CharacterController::Action::MOVE_BACKWARD);
        auto jumpState = std::find_if(states_.begin(), states_.end(),
                                      [](const auto& state) { return state == CharacterController::Action::JUMP; });

        if (jumpState == states_.end())
        {
            animator_->ChangeAnimation(idleAnimationName_, Animator::AnimationChangeType::smooth);
        }
    });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        KeyCodes::A, [&]() { RemoveState(CharacterController::Action::ROTATE_LEFT); });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        KeyCodes::D, [&]() { RemoveState(CharacterController::Action::ROTATE_RIGHT); });
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
}  // namespace Components
}  // namespace GameEngine
