#include "mrpg_main.h"
#include "Gateway.h"
#include "Common/MessageHandling/Dispatcher.h"
#include "MrpgGameContext.h"
#include "Engine/Engine.h"
#include "Common/Hero/HeroClassesTypes.h"
#include "Scenes/ScenesFactory.h"
#include "Logger/Log.h"
#include "PlayerMock.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

namespace MmmoRpg
{
	class Game
	{
	public:
		Game(IGraphicsApiPtr gptr)
			: serverAdress(Utils::ReadFile("./server.conf"))
			, engine(gptr, makeBulletAdapter(), std::make_shared<SceneFactory>(gateway, serverAdress, gameContext_))
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
		CEngine engine;
		MrpgGameContext gameContext_;
	};

	void StartGame(GameEngine::IGraphicsApiPtr gptr)
	{
		Game game(gptr);
		//Mock::StartMock();
	}
}