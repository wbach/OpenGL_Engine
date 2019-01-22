#include "PlayerController.h"
#include "Input/InputManager.h"
#include "UtilsNetwork/Gateway.h"
#include "Messages/TransformMessages/TransformMsgReq.h"
#include "TestGame/MRpg/MrpgGameContext.h"
#include <algorithm>
#include <ctime>

namespace MmmoRpg
{
	PlayerController::PlayerController(Input::InputManager* manager, MrpgGameContext& gameContext, Network::CGateway& gateway)
		: gameContext_(gameContext)
		, inputManager_(manager)
		, gateway_(gateway)
	{
		SubscribeForPushActions();
		SubscribeForPopActions();
	}
	void PlayerController::SendTransformMessage(Network::TransformMessageTypes type, Network::TransformAction action)
	{
		if (gameContext_.selectedCharacterId.second != SelectedCharacterState::READY_TO_USE)
			return;

		if (FindState(type) && action == Network::TransformAction::PUSH)
			return;

		Log("Times test : Req: " + std::to_string(clock() * 1000.0f / (float)CLOCKS_PER_SEC) + " Action: " + std::to_string(action));

		auto msg = std::make_unique<Network::TransformMsgReq>();
		msg->type = type;
		msg->id = gameContext_.selectedCharacterId.first;
		msg->action = action;

		gateway_.Send(msg.get());

		if (action == Network::TransformAction::PUSH)
			states_.push_back(type);
		else
			states_.remove(type);
	}
	bool PlayerController::FindState(Network::TransformMessageTypes type)
	{
		return std::find(states_.begin(), states_.end(), type) != states_.end();
	}
	void PlayerController::SubscribeForPushActions()
	{
		inputManager_->SubscribeOnKeyDown(KeyCodes::W, [&]()
		{
			SendTransformMessage(Network::TransformMessageTypes::MOVE_FORWARD, Network::TransformAction::PUSH);
		});
		inputManager_->SubscribeOnKeyDown(KeyCodes::S, [&]()
		{
			SendTransformMessage(Network::TransformMessageTypes::MOVE_BACKWARD, Network::TransformAction::PUSH);
		});
		inputManager_->SubscribeOnKeyDown(KeyCodes::A, [&]()
		{
			SendTransformMessage(Network::TransformMessageTypes::ROTATE_LEFT, Network::TransformAction::PUSH);
		});
		inputManager_->SubscribeOnKeyDown(KeyCodes::D, [&]()
		{
			SendTransformMessage(Network::TransformMessageTypes::ROTATE_RIGHT, Network::TransformAction::PUSH);
		});
	}
	void PlayerController::SubscribeForPopActions()
	{
		inputManager_->SubscribeOnKeyUp(KeyCodes::W, [&]()
		{
			SendTransformMessage(Network::TransformMessageTypes::MOVE_FORWARD, Network::TransformAction::POP);
		});
		inputManager_->SubscribeOnKeyUp(KeyCodes::S, [&]()
		{
			SendTransformMessage(Network::TransformMessageTypes::MOVE_BACKWARD, Network::TransformAction::POP);
		});
		inputManager_->SubscribeOnKeyUp(KeyCodes::A, [&]()
		{
			SendTransformMessage(Network::TransformMessageTypes::ROTATE_LEFT, Network::TransformAction::POP);
		});
		inputManager_->SubscribeOnKeyUp(KeyCodes::D, [&]()
		{
			SendTransformMessage(Network::TransformMessageTypes::ROTATE_RIGHT, Network::TransformAction::POP);
		});
	}
} // MmmoRpg