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
			engine.scene = std::make_shared<LoginScene>(engine, gateway, serverAdress);
			engine.scenes = 
			{
				std::make_shared<LoginScene>(engine, gateway, serverAdress),
				std::make_shared<MainScene>(engine),
				std::make_shared<SelectCharacterScene>(engine, gateway)
			};
			engine.PreperaScene();
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