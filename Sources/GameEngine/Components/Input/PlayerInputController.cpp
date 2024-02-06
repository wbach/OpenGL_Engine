#include "PlayerInputController.h"

#include <Input/InputManager.h>

#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"

using namespace common::Controllers;

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string COMPONENT_STR{"PlayerInputController"};
}  // namespace

PlayerInputController::PlayerInputController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(PlayerInputController).hash_code(), componentContext, gameObject)
    , characterController_{nullptr}
    , subscriptions_{componentContext.inputManager_}
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
    characterController_ = thisObject_.GetComponent<CharacterController>();

    if (characterController_)
    {
        if (not characterController_)
            return;

        SubscribeForPushActions();
        SubscribeForPopActions();
    }
}

void PlayerInputController::SubscribeForPushActions()
{
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::W, [&]() {
        if (componentContext_.inputManager_.GetKey(KeyCodes::LCTRL))
        {
            characterController_->handleEvent(WalkForwardEvent{});
        }
        else if (componentContext_.inputManager_.GetKey(KeyCodes::LSHIFT))
        {
            characterController_->handleEvent(SprintStartEvent{});
        }
        else
        {
            characterController_->handleEvent(RunForwardEvent{});
        }
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::S, [&]() {
        if (componentContext_.inputManager_.GetKey(KeyCodes::LCTRL))
        {
            characterController_->handleEvent(WalkBackwardEvent{});
        }
        else
        {
            characterController_->handleEvent(RunBackwardEvent{});
        }
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::A, [&]() {
        isRotateLeftPressed_ = true;
        characterController_->handleEvent(RotateLeftEvent{});
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::D, [&]() {
        isRotateRightPressed_ = true;
        characterController_->handleEvent(RotateRightEvent{});
    });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::SPACE, [&]() {
        // characterController_->handleEvent(JumpEvent{DEFAULT_JUMP_POWER});
    });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        KeyCodes::LCTRL, [&]() { characterController_->handleEvent(WalkChangeStateEvent{}); });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        KeyCodes::LSHIFT, [&]() { characterController_->handleEvent(SprintStateChangeEvent{}); });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        KeyCodes::LMOUSE, [&]() { characterController_->handleEvent(AttackEvent{}); });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::RMOUSE, [&]() {
        characterController_->handleEvent(DrawArrowEvent{});
        // characterController_->handleEvent(EndAttackEvent{});
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        KeyCodes::F, [&]() { characterController_->handleEvent(WeaponStateEvent{}); });
}

void PlayerInputController::SubscribeForPopActions()
{
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        KeyCodes::W, [&]() { characterController_->handleEvent(EndForwardMoveEvent{}); });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        KeyCodes::S, [&]() { characterController_->handleEvent(EndBackwardMoveEvent{}); });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::A, [&]() {
        isRotateLeftPressed_ = false;

        if (not isRotateRightPressed_)
        {
            characterController_->handleEvent(EndRotationEvent{});
        }
        else
        {
            characterController_->handleEvent(RotateRightEvent{});
        }
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::D, [&]() {
        isRotateRightPressed_ = false;

        if (not isRotateLeftPressed_)
        {
            characterController_->handleEvent(EndRotationEvent{});
        }
        else
        {
            characterController_->handleEvent(RotateLeftEvent{});
        }
    });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        KeyCodes::RMOUSE, [&]() { characterController_->handleEvent(AimStopEvent{}); });
}

void PlayerInputController::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        return std::make_unique<PlayerInputController>(componentContext, gameObject);
    };

    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}

void PlayerInputController::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
}

}  // namespace Components
}  // namespace GameEngine
