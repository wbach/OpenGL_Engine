#include <algorithm>
#include <ctime>
#include "Input/InputManager.h"
#include "Common/Messages/TransformMessages/TransformMsgReq.h"
#include "PlayerController.h"
#include "TestGame/MRpg/MrpgGameContext.h"
#include "UtilsNetwork/Gateway.h"

namespace MmmoRpg
{
PlayerController::PlayerController(Input::InputManager* manager, MrpgGameContext& gameContext,
                                   Network::Gateway& gateway)
    : gameContext_(gameContext)
    , inputManager_(manager)
    , gateway_(gateway)
{
    SubscribeForPushActions();
    SubscribeForPopActions();
}
void PlayerController::SendTransformMessage(common::TransformMessageTypes type, common::TransformAction action)
{
    if (gameContext_.selectedCharacterId.second != SelectedCharacterState::READY_TO_USE)
        return;

    if (FindState(type) && action == common::TransformAction::PUSH)
        return;

    DEBUG_LOG("Times test : Req: " + std::to_string(clock() * 1000.0f / (float)CLOCKS_PER_SEC) +
              " Action: " + std::to_string(action));

    auto msg    = std::make_unique<common::TransformMsgReq>();
    msg->type   = type;
    msg->id     = gameContext_.selectedCharacterId.first;
    msg->action = action;

    gateway_.Send(*msg);

    if (action == common::TransformAction::PUSH)
        states_.push_back(type);
    else
        states_.remove(type);
}
bool PlayerController::FindState(common::TransformMessageTypes type)
{
    return std::find(states_.begin(), states_.end(), type) != states_.end();
}
void PlayerController::SubscribeForPushActions()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::W, [&]() {
        SendTransformMessage(common::TransformMessageTypes::MOVE_FORWARD, common::TransformAction::PUSH);
    });
    inputManager_->SubscribeOnKeyDown(KeyCodes::S, [&]() {
        SendTransformMessage(common::TransformMessageTypes::MOVE_BACKWARD, common::TransformAction::PUSH);
    });
    inputManager_->SubscribeOnKeyDown(KeyCodes::A, [&]() {
        SendTransformMessage(common::TransformMessageTypes::ROTATE_LEFT, common::TransformAction::PUSH);
    });
    inputManager_->SubscribeOnKeyDown(KeyCodes::D, [&]() {
        SendTransformMessage(common::TransformMessageTypes::ROTATE_RIGHT, common::TransformAction::PUSH);
    });
}
void PlayerController::SubscribeForPopActions()
{
    inputManager_->SubscribeOnKeyUp(KeyCodes::W, [&]() {
        SendTransformMessage(common::TransformMessageTypes::MOVE_FORWARD, common::TransformAction::POP);
    });
    inputManager_->SubscribeOnKeyUp(KeyCodes::S, [&]() {
        SendTransformMessage(common::TransformMessageTypes::MOVE_BACKWARD, common::TransformAction::POP);
    });
    inputManager_->SubscribeOnKeyUp(KeyCodes::A, [&]() {
        SendTransformMessage(common::TransformMessageTypes::ROTATE_LEFT, common::TransformAction::POP);
    });
    inputManager_->SubscribeOnKeyUp(KeyCodes::D, [&]() {
        SendTransformMessage(common::TransformMessageTypes::ROTATE_RIGHT, common::TransformAction::POP);
    });
}
}  // namespace MmmoRpg
