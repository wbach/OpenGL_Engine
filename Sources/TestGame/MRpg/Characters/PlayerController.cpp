#include "PlayerController.h"
#include "../../../UtilsNetwork/Gateway.h"
#include "../../../GameEngine/Input/InputManager.h"

namespace MmmoRpg
{
	PlayerController::PlayerController(GameEngine::InputManager* manager, uint32& characterId, Network::CGateway& gateway)
		: characterId_(characterId)
		, inputManager_(manager)
		, gateway_(gateway)
	{
	}
	void PlayerController::Control()
	{
		if (inputManager_->GetKey(KeyCodes::D))
		{

		}
	}
} // MmmoRpg