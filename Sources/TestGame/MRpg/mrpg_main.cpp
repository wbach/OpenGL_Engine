#include "mrpg_main.h"
#include "Gateway.h"
#include "Common/MessageHandling/Dispatcher.h"
#include "MrpgGameContext.h"
#include "Engine/Engine.h"
#include "Engine/Configuration.h"
#include "Api/OpenGL/OpenGLApi.h"
#include "Common/Hero/HeroClassesTypes.h"
#include "Scenes/ScenesFactory.h"
#include "Logger/Log.h"

#include "PlayerMock.h"

namespace MmmoRpg
{
	class Game
	{
	public:
		Game()
			: serverAdress(Utils::ReadFile("./server.conf"))
			, engine(std::make_shared<GameEngine::OpenGLApi>(), std::make_shared<SceneFactory>(gateway, serverAdress, gameContext_))
		{
			Log("Server : " + serverAdress);

			engine.Init();
			//LoginScene
			//engine.sceneManager_.SetActiveScene("GuiEdytorScene");
			engine.sceneManager_.SetActiveScene("LoginScene");
			//engine.sceneManager_.SetActiveScene("SelectCharacterScene");
			engine.GameLoop();
		}
	private:
		std::string serverAdress;
		Network::CGateway gateway;
		GameEngine::CEngine engine;
		MrpgGameContext gameContext_;
	};

	void StartGame()
	{
		Game game;
		//Mock::StartMock();
	}
}