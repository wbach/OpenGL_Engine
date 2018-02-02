#include "ScenesFactory.h"
#include "Login.h"
#include "SelectCharacter/SelectCharacterScene.h"
#include "MainScene/MainRpgScene.h"
#include "../../GuiEdytorScene.h"
#include "Logger/Log.h"

namespace MmmoRpg
{
	SceneFactory::SceneFactory(Network::CGateway & gateway, std::string & serverAdress)
		: gateway_(gateway)
		, serverAdress_(serverAdress)
	{
		AddScene("LoginScene", std::bind(&SceneFactory::CreateLoginScene, this));
		AddScene("SelectCharacterScene", std::bind(&SceneFactory::CreateSelectCharacterScene, this));
		AddScene("MainRpgScene", std::bind(&SceneFactory::CreateMainScene, this));
		AddScene("GuiEdytorScene", std::bind(&SceneFactory::CreateGuiEditorScene, this));
	}
	
	GameEngine::ScenePtr SceneFactory::CreateMainScene()
	{
		return std::make_unique<MainRpgScene>(gateway_);
	}
	GameEngine::ScenePtr SceneFactory::CreateLoginScene()
	{
		return std::make_unique<LoginScene>(gateway_, serverAdress_);
	}
	GameEngine::ScenePtr SceneFactory::CreateSelectCharacterScene()
	{
		return std::make_unique<SelectCharacterScene>(gateway_);
	}
	GameEngine::ScenePtr SceneFactory::CreateGuiEditorScene()
	{
		return std::make_unique<GuiEdytorScene>();
	}
} // MmmoRpg