#include "PlayerInputController.h"
#include "Input/InputManager.h"
#include "UtilsNetwork/Gateway.h"
#include "Messages/TransformMessages/TransformMsgReq.h"
#include "TestGame/MRpg/MrpgGameContext.h"
#include <algorithm>
#include <ctime>

using namespace common::Controllers::CharacterActions;

PlayerInputController::PlayerInputController(GameEngine::InputManager* manager, common::Controllers::CharacterController* characterController)
	: inputManager_(manager)
	, characterController_(characterController)
{
	SubscribeForPushActions();
	SubscribeForPopActions();
}

void PlayerInputController::SubscribeForPushActions()
{
	inputManager_->SubscribeOnKeyDown(KeyCodes::W, [&]()
	{
		if (FindState(MOVE_FORWARD))
			return;

		characterController_->AddState(MOVE_FORWARD);
		states_.push_back(MOVE_FORWARD);
	});
	inputManager_->SubscribeOnKeyDown(KeyCodes::S, [&]()
	{
		if (FindState(MOVE_BACKWARD))
			return;
		characterController_->AddState(MOVE_BACKWARD);
		states_.push_back(MOVE_BACKWARD);
	});
	inputManager_->SubscribeOnKeyDown(KeyCodes::A, [&]()
	{
		if (FindState(ROTATE_LEFT))
			return;
		characterController_->AddState(ROTATE_LEFT);
		states_.push_back(ROTATE_LEFT);
	});
	inputManager_->SubscribeOnKeyDown(KeyCodes::D, [&]()
	{
		if (FindState(ROTATE_RIGHT))
			return;
		characterController_->AddState(ROTATE_RIGHT);
		states_.push_back(ROTATE_RIGHT);
	});
}
void PlayerInputController::SubscribeForPopActions()
{
	inputManager_->SubscribeOnKeyUp(KeyCodes::W, [&]()
	{
		if (!FindState(MOVE_FORWARD))
			return;
		characterController_->RemoveState(MOVE_FORWARD);
		states_.remove(MOVE_FORWARD);
	});
	inputManager_->SubscribeOnKeyUp(KeyCodes::S, [&]()
	{
		if (!FindState(MOVE_FORWARD))
			return;
		characterController_->RemoveState(MOVE_FORWARD);
		states_.remove(MOVE_FORWARD);

		characterController_->RemoveState(common::Controllers::CharacterActions::MOVE_BACKWARD);
	});
	inputManager_->SubscribeOnKeyUp(KeyCodes::A, [&]()
	{
		if (!FindState(ROTATE_LEFT))
			return;
		characterController_->RemoveState(ROTATE_LEFT);
		states_.remove(ROTATE_LEFT);
	});
	inputManager_->SubscribeOnKeyUp(KeyCodes::D, [&]()
	{
		if (!FindState(ROTATE_RIGHT))
			return;
		characterController_->RemoveState(ROTATE_RIGHT);
		states_.remove(ROTATE_RIGHT);
	});
}

bool PlayerInputController::FindState(common::Controllers::CharacterActions::Type type)
{
	return std::find(states_.begin(), states_.end(), type) != states_.end();;
}
