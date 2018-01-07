#include "MainScene.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"
#include <fstream>
#include "FreeImage.h"


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
	TestGameStart();
	return 0;
}
