#pragma once
#include "Scene/Scene.hpp"
#include "Common/MessageHandling/Dispatcher.h"
#include "TestGame/MRpg/MrpgGameContext.h"
#include <unordered_map>

namespace Network
{
	class CGateway;
} // Network

namespace MmmoRpg
{
	class MRpgScene : public GameEngine::Scene
	{
	public:
		MRpgScene(const std::string& name, Network::CGateway& gateway, const std::string& serverAddress, MrpgGameContext& gameContext);
		virtual ~MRpgScene() override;

	protected:
		std::string serverAddress_;
		Network::CGateway& gateway_;
		MrpgGameContext& gameContext_;
		common::Dispacher dispatcher_;
	};
} // MmmoRpg