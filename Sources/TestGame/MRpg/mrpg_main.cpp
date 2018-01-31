#include "mrpg_main.h"
#include "Logger/Log.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Engine/Configuration.h"
#include "../UtilsNetwork/Gateway.h"
#include "Scenes/ScenesFactory.h"

namespace MmmoRpg
{
	class Game
	{
	public:
		Game()
			: engine(std::make_shared<SceneFactory>(gateway, serverAdress))
		{
			std::string  serverAdress = Utils::ReadFile("./server.conf");
			Log("Server : " + serverAdress);

			engine.Init();
			//LoginScene
			//engine.sceneManager_.SetActiveScene("GuiEdytorScene");
			engine.sceneManager_.SetActiveScene("LoginScene");
			//engine.sceneManager_.SetActiveScene("SelectCharacterScene");
			engine.GameLoop();
		}
	private:
		Network::CGateway gateway;
		GameEngine::CEngine engine;
		std::string serverAdress;
	};

	void StartGame()
	{
		CLogger::Instance().EnableLogs();
		Game game;
	}
}