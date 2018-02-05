#pragma once
#include "../../../Common/Hero/Stats.h"
#include "../../../UtilsNetwork/Messages/TransformMessages/TransformMessageTypes.h"

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
		PlayerController(GameEngine::InputManager* manager, uint32 characterId, Network::CGateway& gateway);

	private:
		void SendTransformMessage(Network::TransformMessageTypes type, Network::TransformAction action);
		bool FindState(Network::TransformMessageTypes type);
		void SubscribeForPushActions();
		void SubscribeForPopActions();
	private:
		uint32 characterId_;
		GameEngine::InputManager* inputManager_;
		Network::CGateway& gateway_;
		std::list<Network::TransformMessageTypes> states_;
	};
} // MmmoRpg