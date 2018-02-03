#pragma once
#include "../../../Common/Hero/Stats.h"

class CInputManager;
namespace Network
{
	class CGateway;
}

namespace MmmoRpg
{
	class PlayerController
	{
	public:
		PlayerController(CInputManager* manager, uint32& characterId, Network::CGateway& gateway);
		void Control();

	private:
		uint32& characterId_;
		CInputManager* inputManager_;
		Network::CGateway& gateway_;
	};
} // MmmoRpg