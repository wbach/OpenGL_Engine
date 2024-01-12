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
        auto fsm = characterController_->fsm();
        if (fsm)
        {
            fsm->handle(MoveForwardEvent{});
        }
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::S, [&]() {
        auto fsm = characterController_->fsm();
        if (fsm)
        {
            fsm->handle(MoveBackwardEvent{});
        }
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::A, [&]() {
        isRotateLeftPressed_ = true;
        auto fsm             = characterController_->fsm();
        if (fsm)
        {
            fsm->handle(RotateLeftEvent{});
        }
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::D, [&]() {
        isRotateRightPressed_ = true;
        auto fsm              = characterController_->fsm();
        if (fsm)
        {
            fsm->handle(RotateRightEvent{});
        }
    });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::SPACE, [&]() {
        auto fsm = characterController_->fsm();
        if (fsm)
        {
            fsm->handle(JumpEvent{DEFAULT_JUMP_POWER});
        }
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [&]() {
        auto fsm = characterController_->fsm();
        if (fsm)
        {
            fsm->handle(AttackEvent{});
        }
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::RMOUSE, [&]() {
        auto fsm = characterController_->fsm();
        if (fsm)
        {
            fsm->handle(EndAttackEvent{});
        }
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::F, [&]() {
        auto fsm = characterController_->fsm();
        if (fsm)
        {
            fsm->handle(WeaponStateEvent{});
        }
    });
}

void PlayerInputController::SubscribeForPopActions()
{
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::W, [&]() {
        auto fsm = characterController_->fsm();
        if (fsm)
        {
            fsm->handle(EndMoveEvent{});
        }
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::S, [&]() {
        auto fsm = characterController_->fsm();
        if (fsm)
        {
            fsm->handle(EndMoveEvent{});
        }
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::A, [&]() {
        auto fsm             = characterController_->fsm();
        isRotateLeftPressed_ = false;

        if (fsm)
        {
            if (not isRotateRightPressed_)
            {
                fsm->handle(EndRotationEvent{});
            }
            else
            {
                fsm->handle(RotateRightEvent{});
            }
        }
    });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(KeyCodes::D, [&]() {
        auto fsm              = characterController_->fsm();
        isRotateRightPressed_ = false;
        if (fsm)
        {
            if (not isRotateLeftPressed_)
            {
                fsm->handle(EndRotationEvent{});
            }
            else
            {
                fsm->handle(RotateLeftEvent{});
            }
        }
    });
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
