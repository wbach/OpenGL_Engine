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
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::MOVE_FORWARD,
        [&]()
        {
            characterController_->handleEvent(MoveForwardEvent{});

            if (componentContext_.inputManager_.GetKey(Input::GameAction::WALK))
            {
                characterController_->handleEvent(WalkChangeStateEvent{});
            }
            else if (componentContext_.inputManager_.GetKey(Input::GameAction::SPRINT))
            {
                characterController_->handleEvent(SprintStateChangeEvent{});
            }
        });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::MOVE_BACKWARD,
        [&]()
        {
            characterController_->handleEvent(MoveBackwardEvent{});

            if (componentContext_.inputManager_.GetKey(Input::GameAction::WALK))
            {
                characterController_->handleEvent(WalkChangeStateEvent{});
            }
            else if (componentContext_.inputManager_.GetKey(Input::GameAction::SPRINT))
            {
                characterController_->handleEvent(SprintStateChangeEvent{});
            }
        });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(Input::GameAction::TURN_LEFT,
                                                                        [&]()
                                                                        {
                                                                            isRotateLeftPressed_ = true;
                                                                            characterController_->handleEvent(RotateLeftEvent{});
                                                                        });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(Input::GameAction::TURN_RIGHT,
                                                                        [&]()
                                                                        {
                                                                            isRotateRightPressed_ = true;
                                                                            characterController_->handleEvent(RotateRightEvent{});
                                                                        });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::MOVE_LEFT,
        [&]()
        {
            characterController_->handleEvent(MoveLeftEvent{});

            if (componentContext_.inputManager_.GetKey(Input::GameAction::WALK))
            {
                characterController_->handleEvent(WalkChangeStateEvent{});
            }
            else if (componentContext_.inputManager_.GetKey(Input::GameAction::SPRINT))
            {
                characterController_->handleEvent(SprintStateChangeEvent{});
            }
        });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::MOVE_RIGHT,
        [&]()
        {
            characterController_->handleEvent(MoveRightEvent{});

            if (componentContext_.inputManager_.GetKey(Input::GameAction::WALK))
            {
                characterController_->handleEvent(WalkChangeStateEvent{});
            }
            else if (componentContext_.inputManager_.GetKey(Input::GameAction::SPRINT))
            {
                characterController_->handleEvent(SprintStateChangeEvent{});
            }
        });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::JUMP, [&]() { characterController_->handleEvent(JumpEvent{DEFAULT_JUMP_POWER}); });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::WALK, [&]() { characterController_->handleEvent(WalkChangeStateEvent{}); });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::SPRINT, [&]() { characterController_->handleEvent(SprintStateChangeEvent{}); });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::ATTACK, [&]() { characterController_->handleEvent(AttackEvent{}); });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::AIM, [&]() { characterController_->handleEvent(DrawArrowEvent{}); });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::EQUIP_DISARM, [&]() { characterController_->handleEvent(WeaponStateEvent{}); });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyDown(
        Input::GameAction::CROUCH, [&]() { characterController_->handleEvent(CrouchChangeStateEvent{}); });
}

void PlayerInputController::SubscribeForPopActions()
{
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        Input::GameAction::MOVE_FORWARD, [&]() { characterController_->handleEvent(EndForwardMoveEvent{}); });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        Input::GameAction::MOVE_BACKWARD, [&]() { characterController_->handleEvent(EndBackwardMoveEvent{}); });

    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        Input::GameAction::MOVE_LEFT, [&]() { characterController_->handleEvent(EndMoveLeftEvent{}); });
    subscriptions_ = componentContext_.inputManager_.SubscribeOnKeyUp(
        Input::GameAction::MOVE_RIGHT, [&]() { characterController_->handleEvent(EndMoveRightEvent{}); });

    subscriptions_ =
        componentContext_.inputManager_.SubscribeOnKeyUp(Input::GameAction::TURN_LEFT,
                                                         [&]()
                                                         {
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
    subscriptions_ =
        componentContext_.inputManager_.SubscribeOnKeyUp(Input::GameAction::TURN_RIGHT,
                                                         [&]()
                                                         {
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
        Input::GameAction::AIM, [&]() { characterController_->handleEvent(AimStopEvent{}); });
}

void PlayerInputController::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    { return std::make_unique<PlayerInputController>(componentContext, gameObject); };

    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}

void PlayerInputController::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
}

}  // namespace Components
}  // namespace GameEngine
