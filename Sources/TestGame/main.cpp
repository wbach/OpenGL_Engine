#include "MainScene.h"
#include "MRpg/mrpg_main.h"
#include "../GameEngine/Engine/Engine.h"
#include "Logger/Log.h"



void TestGameStart()
{
	//GameEngine::CEngine engine;
	//engine.Init();
	//engine.sceneManager_.AddScene(new MainScene(engine));
	//engine.sceneManager_.SetActiveScene("MainScene");
	//engine.GameLoop();
}

int main(int argc, char* argv[])
{	
	#ifdef BACHU_HAHAH
	std::cout << "DBACHU_HAHAH MINGW32 build." << std::endl;
	#endif
	//CLogger::Instance().EnableLogs();	TestGameStart();

	MmmoRpg::StartGame();
	return 0;
}
