#include "MainScene.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"
#include <fstream>
#include "FreeImage.h"

void TestGameStart()
{
    CEngine engine;
	engine.Init();
    engine.scene = std::make_unique<MainScene>(engine);
	engine.PreperaScene();
	engine.GameLoop();
}



int main(int argc, char* argv[])
{	
	CLogger::Instance().EnableLogs();
	TestGameStart();
	return 0;
}
