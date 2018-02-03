#pragma once
#include "../../../Common/Hero/Stats.h"

namespace GameEngine
{
	class InputManager;
}

namespace Network
{
	class CGateway;
}

namespace MmmoRpg
{
	class PlayerController
	{
	public:
		PlayerController(GameEngine::InputManager* manager, uint32& characterId, Network::CGateway& gateway);
		void Control();

	private:
		uint32& characterId_;
		GameEngine::InputManager* inputManager_;
		Network::CGateway& gateway_;
	};
} // MmmoRpg