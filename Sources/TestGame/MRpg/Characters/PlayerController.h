#pragma once
#include "Common/Hero/Stats.h"
#include "UtilsNetwork/Messages/TransformMessages/TransformMessageTypes.h"
#include <memory>

namespace Input
{
	class InputManager;
}

namespace Network
{
	class CGateway;
}

namespace MmmoRpg
{
	struct MrpgGameContext;

	class PlayerController
	{
	public:
		PlayerController(Input::InputManager* manager, MrpgGameContext& gameContext, Network::CGateway& gateway);

	private:
		void SendTransformMessage(Network::TransformMessageTypes type, Network::TransformAction action);
		bool FindState(Network::TransformMessageTypes type);
		void SubscribeForPushActions();
		void SubscribeForPopActions();
	private:
		MrpgGameContext& gameContext_;
		Input::InputManager* inputManager_;
		Network::CGateway& gateway_;
		std::list<Network::TransformMessageTypes> states_;
	};

	typedef std::unique_ptr<PlayerController> PlayerControllerPtr;
} // MmmoRpg