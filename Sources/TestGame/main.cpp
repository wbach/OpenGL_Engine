#include "MainScene.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"
#include <fstream>
#include "FreeImage.h"
#include "../UtilsNetwork/Gateway.h"
#include <conio.h>

void TestGameStart()
{
	GameEngine::CEngine engine;
	engine.Init();
    engine.scene = std::make_unique<MainScene>(engine);
	engine.PreperaScene();
	engine.GameLoop();
}

int main(int argc, char* argv[])
{	
	#ifdef BACHU_HAHAH
	std::cout << "DBACHU_HAHAH MINGW32 build." << std::endl;
	#endif
	CLogger::Instance().EnableLogs();
	
	Network::CGateway gateway;
	gateway.ConnectToServer("baszek", "haslo", 1991);

	while (true)
	{
		Log("ClientMainLoop.");
		auto msg = gateway.PopInBox();
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	_getch();
	return 0;
	TestGameStart();
	return 0;
}
