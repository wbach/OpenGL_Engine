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
    , animator_{nullptr}
    , characterController_{nullptr}
    , subscriptions_{componentContext.inputManager_}
{
}

void PlayerInputController::CleanUp()
{
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
        if (not characterController_)
            return;

        SubscribeForPushActions();
        SubscribeForPopActions();
    }
}

void PlayerInputController::SubscribeForPushActions()
{
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::W, [&]() {
        characterController_->addState(std::make_unique<MoveForward>());
        characterController_->removeState(CharacterControllerState::Type::MOVE_BACKWARD);
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::S, [&]() {
        characterController_->addState(std::make_unique<MoveBackward>());
        characterController_->removeState(CharacterControllerState::Type::MOVE_FORWARD);
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::A, [&]() {
        characterController_->addState(std::make_unique<RotateLeft>());
        characterController_->removeState(CharacterControllerState::Type::ROTATE_RIGHT);
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::D, [&]() {
        characterController_->addState(std::make_unique<RotateRight>());
        characterController_->removeState(CharacterControllerState::Type::ROTATE_LEFT);
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        KeyCodes::SPACE, [&]() { characterController_->addState(std::make_unique<Jump>(DEFAULT_JUMP_POWER)); });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        KeyCodes::LMOUSE, [&]() { characterController_->addState(std::make_unique<Attack>()); });
}

void PlayerInputController::SubscribeForPopActions()
{
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        KeyCodes::W, [&]() { characterController_->removeState(CharacterControllerState::Type::MOVE_FORWARD); });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        KeyCodes::S, [&]() { characterController_->removeState(CharacterControllerState::Type::MOVE_BACKWARD); });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        KeyCodes::A, [&]() { characterController_->removeState(CharacterControllerState::Type::ROTATE_LEFT); });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        KeyCodes::D, [&]() { characterController_->removeState(CharacterControllerState::Type::ROTATE_RIGHT); });
}

}  // namespace Components
}  // namespace GameEngine
