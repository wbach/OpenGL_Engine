#pragma once
#include "Scene/SceneFactoryBase.h"
#include "../MrpgGameContext.h"

namespace Network
{
	class CGateway;
} // Network

namespace MmmoRpg
{	
	class SceneFactory : public GameEngine::SceneFactoryBase
	{
	public:
		SceneFactory(Network::CGateway& gateway, std::string& serverAdress, MrpgGameContext& gameContext);

	private:
		GameEngine::ScenePtr CreateMainScene();
		GameEngine::ScenePtr CreateLoginScene();
		GameEngine::ScenePtr CreateSelectCharacterScene();
		GameEngine::ScenePtr CreateGuiEditorScene();

	private:
		Network::CGateway& gateway_;
		std::string& serverAdress_;
		MrpgGameContext& gameContext_;
	};
}