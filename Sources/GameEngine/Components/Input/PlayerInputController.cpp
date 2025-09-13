#include "PlayerInputController.h"

#include <Input/GameActions.h>
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
    : BaseComponent(GetComponentType<PlayerInputController>(), componentContext, gameObject)
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
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::DODGE,
        [&]()
        {
            if (componentContext_.inputManager_.GetKey(Input::GameAction::MOVE_FORWARD))
            {
                characterController_->pushEventToQueue(DodgeForwardEvent{DEFAULT_DODGE_POWER});
                return;
            }
            if (componentContext_.inputManager_.GetKey(Input::GameAction::MOVE_BACKWARD))
            {
                characterController_->pushEventToQueue(DodgeBackwardEvent{DEFAULT_DODGE_POWER});
                return;
            }
            if (componentContext_.inputManager_.GetKey(Input::GameAction::MOVE_LEFT))
            {
                characterController_->pushEventToQueue(DodgeLeftEvent{DEFAULT_DODGE_POWER});
                return;
            }
            if (componentContext_.inputManager_.GetKey(Input::GameAction::MOVE_RIGHT))
            {
                characterController_->pushEventToQueue(DodgeRightEvent{DEFAULT_DODGE_POWER});
                return;
            }
        });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::MOVE_FORWARD,
        [&]()
        {
            if (componentContext_.inputManager_.GetKey(Input::GameAction::DODGE))
            {
                characterController_->pushEventToQueue(DodgeForwardEvent{DEFAULT_DODGE_POWER});
                return;
            }

            characterController_->pushEventToQueue(MoveForwardEvent{});

            if (componentContext_.inputManager_.GetKey(Input::GameAction::WALK))
            {
                characterController_->pushEventToQueue(WalkChangeStateEvent{});
            }
            else if (componentContext_.inputManager_.GetKey(Input::GameAction::SPRINT))
            {
                characterController_->pushEventToQueue(SprintStateChangeEvent{});
            }
        });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::MOVE_BACKWARD,
        [&]()
        {
            if (componentContext_.inputManager_.GetKey(Input::GameAction::DODGE))
            {
                characterController_->pushEventToQueue(DodgeBackwardEvent{DEFAULT_DODGE_POWER});
                return;
            }
            characterController_->pushEventToQueue(MoveBackwardEvent{});

            if (componentContext_.inputManager_.GetKey(Input::GameAction::WALK))
            {
                characterController_->pushEventToQueue(WalkChangeStateEvent{});
            }
            else if (componentContext_.inputManager_.GetKey(Input::GameAction::SPRINT))
            {
                characterController_->pushEventToQueue(SprintStateChangeEvent{});
            }
        });
    subscriptions_ =
        componentContext_.inputManager_.SubscribeOnKeyDown(Input::GameAction::TURN_LEFT,
                                                           [&]()
                                                           {
                                                               isRotateLeftPressed_ = true;
                                                               characterController_->pushEventToQueue(RotateLeftEvent{});
                                                           });
    subscriptions_ =
        componentContext_.inputManager_.SubscribeOnKeyDown(Input::GameAction::TURN_RIGHT,
                                                           [&]()
                                                           {
                                                               isRotateRightPressed_ = true;
                                                               characterController_->pushEventToQueue(RotateRightEvent{});
                                                           });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::MOVE_LEFT,
        [&]()
        {
            if (componentContext_.inputManager_.GetKey(Input::GameAction::DODGE))
            {
                characterController_->pushEventToQueue(DodgeLeftEvent{DEFAULT_DODGE_POWER});
                return;
            }
            characterController_->pushEventToQueue(MoveLeftEvent{});

            if (componentContext_.inputManager_.GetKey(Input::GameAction::WALK))
            {
                characterController_->pushEventToQueue(WalkChangeStateEvent{});
            }
            else if (componentContext_.inputManager_.GetKey(Input::GameAction::SPRINT))
            {
                characterController_->pushEventToQueue(SprintStateChangeEvent{});
            }
        });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::MOVE_RIGHT,
        [&]()
        {
            if (componentContext_.inputManager_.GetKey(Input::GameAction::DODGE))
            {
                characterController_->pushEventToQueue(DodgeRightEvent{DEFAULT_DODGE_POWER});
                return;
            }
            characterController_->pushEventToQueue(MoveRightEvent{});

            if (componentContext_.inputManager_.GetKey(Input::GameAction::WALK))
            {
                characterController_->pushEventToQueue(WalkChangeStateEvent{});
            }
            else if (componentContext_.inputManager_.GetKey(Input::GameAction::SPRINT))
            {
                characterController_->pushEventToQueue(SprintStateChangeEvent{});
            }
        });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::JUMP, [&]() { characterController_->pushEventToQueue(JumpEvent{DEFAULT_JUMP_POWER}); });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::DODGE_DIVE,
        [&]() { characterController_->pushEventToQueue(DodgeDiveEvent{DEFAULT_DODGE_DIVE_POWER}); });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::WALK, [&]() { characterController_->pushEventToQueue(WalkChangeStateEvent{}); });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::SPRINT, [&]() { characterController_->pushEventToQueue(SprintStateChangeEvent{}); });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::ATTACK, [&]() { characterController_->pushEventToQueue(AttackEvent{}); });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::AIM, [&]() { characterController_->pushEventToQueue(DrawArrowEvent{}); });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::EQUIP_DISARM, [&]() { characterController_->pushEventToQueue(WeaponStateEvent{}); });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::CROUCH, [&]() { characterController_->pushEventToQueue(CrouchChangeStateEvent{}); });
}

void PlayerInputController::SubscribeForPopActions()
{
    subscriptions_ =
        componentContext_.inputManager_.SubscribeOnKeyUp(Input::GameAction::MOVE_FORWARD,
                                                         [&]()
                                                         {
                                                             if (componentContext_.inputManager_.GetKey(Input::GameAction::DODGE))
                                                             {
                                                                 return;
                                                             }
                                                             characterController_->pushEventToQueue(EndForwardMoveEvent{});
                                                         });
    subscriptions_ =
        componentContext_.inputManager_.SubscribeOnKeyUp(Input::GameAction::MOVE_BACKWARD,
                                                         [&]()
                                                         {
                                                             if (componentContext_.inputManager_.GetKey(Input::GameAction::DODGE))
                                                             {
                                                                 return;
                                                             }
                                                             characterController_->pushEventToQueue(EndBackwardMoveEvent{});
                                                         });

    subscriptions_ =
        componentContext_.inputManager_.SubscribeOnKeyUp(Input::GameAction::MOVE_LEFT,
                                                         [&]()
                                                         {
                                                             if (componentContext_.inputManager_.GetKey(Input::GameAction::DODGE))
                                                             {
                                                                 return;
                                                             }
                                                             characterController_->pushEventToQueue(EndMoveLeftEvent{});
                                                         });
    subscriptions_ =
        componentContext_.inputManager_.SubscribeOnKeyUp(Input::GameAction::MOVE_RIGHT,
                                                         [&]()
                                                         {
                                                             if (componentContext_.inputManager_.GetKey(Input::GameAction::DODGE))
                                                             {
                                                                 return;
                                                             }
                                                             characterController_->pushEventToQueue(EndMoveRightEvent{});
                                                         });

    subscriptions_ =
        componentContext_.inputManager_.SubscribeOnKeyUp(Input::GameAction::TURN_LEFT,
                                                         [&]()
                                                         {
                                                             isRotateLeftPressed_ = false;

                                                             if (not isRotateRightPressed_)
                                                             {
                                                                 characterController_->pushEventToQueue(EndRotationEvent{});
                                                             }
                                                             else
                                                             {
                                                                 characterController_->pushEventToQueue(RotateRightEvent{});
                                                             }
                                                         });
    subscriptions_ =
        componentContext_.inputManager_.SubscribeOnKeyUp(Input::GameAction::TURN_RIGHT,
                                                         [&]()
                                                         {
                                                             isRotateRightPressed_ = false;

                                                             if (not isRotateLeftPressed_)
                                                             {
                                                                 characterController_->pushEventToQueue(EndRotationEvent{});
                                                             }
                                                             else
                                                             {
                                                                 characterController_->pushEventToQueue(RotateLeftEvent{});
                                                             }
                                                         });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        Input::GameAction::AIM, [&]() { characterController_->pushEventToQueue(AimStopEvent{}); });
}

void PlayerInputController::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    { return std::make_unique<PlayerInputController>(componentContext, gameObject); };

    regsiterComponentReadFunction(GetComponentType<PlayerInputController>(), readFunc);
}

void PlayerInputController::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
}

}  // namespace Components
}  // namespace GameEngine
