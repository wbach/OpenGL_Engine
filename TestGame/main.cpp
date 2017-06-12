#include "MainScene.h"
#include "../GameEngine/Engine/Engine.h"

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
	//MemoryTest();
	CLogger::Instance().EnableLogs();
	TestGameStart();
	return 0;
}
