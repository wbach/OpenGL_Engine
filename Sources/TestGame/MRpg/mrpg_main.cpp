#include "mrpg_main.h"
#include "Logger/Log.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Engine/Configuration.h"
#include "../UtilsNetwork/Gateway.h"
#include "Scenes/Login.h"
#include "Scenes/SelectCharacter/SelectCharacterScene.h"
#include "../MainScene.h"

namespace MmmoRpg
{
	class Game
	{
	public:
		Game()
		{
			std::string  serverAdress = Utils::ReadFile("./server.conf");
			Log("Server : " + serverAdress);

			engine.Init();
			engine.sceneManager_.AddScene(new LoginScene(engine, gateway, serverAdress));
			engine.sceneManager_.AddScene(new MainScene(engine));
			engine.sceneManager_.AddScene(new SelectCharacterScene(engine, gateway));
			engine.sceneManager_.SetActiveScene("LoginScene");
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