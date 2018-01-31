#pragma once
#include "Scene/SceneFactoryBase.h"

namespace Network
{
	class CGateway;
} // Network

namespace MmmoRpg
{	
	class SceneFactory : public GameEngine::SceneFactoryBase
	{
	public:
		SceneFactory(Network::CGateway& gateway, std::string& serverAdress);

	private:
		GameEngine::ScenePtr CreateMainScene();
		GameEngine::ScenePtr CreateLoginScene();
		GameEngine::ScenePtr CreateSelectCharacterScene();
		GameEngine::ScenePtr CreateGuiEditorScene();

	private:
		Network::CGateway& gateway_;
		std::string& serverAdress_;		
	};
}