#include "PlayerController.h"
#include "Input/InputManager.h"
#include "UtilsNetwork/Gateway.h"
#include "Messages/TransformMessages/TransformMsgReq.h"
#include <algorithm>
#include <ctime>

namespace MmmoRpg
{
	PlayerController::PlayerController(GameEngine::InputManager* manager, uint32 characterId, Network::CGateway& gateway)
		: characterId_(characterId)
		, inputManager_(manager)
		, gateway_(gateway)
	{
		SubscribeForPushActions();
		SubscribeForPopActions();
	}
	void PlayerController::SendTransformMessage(Network::TransformMessageTypes type, Network::TransformAction action)
	{
		if (FindState(type) && action == Network::TransformAction::PUSH)
			return;

		Log("Times test : Req: " + std::to_string(clock() * 1000.0f / (float)CLOCKS_PER_SEC) + " Action: " + std::to_string(action));

	/*	Network::TransformMsgReq msg;
		msg.type = type;
		msg.id = characterId_;
		msg.action = action;
		*/

		auto msg = std::make_unique<Network::TransformMsgReq>();
		msg->type = type;
		msg->id = characterId_;
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