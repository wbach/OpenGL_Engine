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
const char serverAdressFilename[] = "./server.conf";



int main(int argc, char* argv[])
{	
	#ifdef BACHU_HAHAH
	std::cout << "DBACHU_HAHAH MINGW32 build." << std::endl;
	#endif
	CLogger::Instance().EnableLogs();
	
	std::string  serverAdress  = Utils::ReadFile(serverAdressFilename);
	Log("Server : " + serverAdress);

	Network::CGateway gateway;
	gateway.ConnectToServer("baszek", "haslo", serverAdress, 1991);


	Network::TransformMsg tdata;
	tdata.position = vec3(1.f, 2.f, 3.f);
	tdata.rotation = vec3(4.f, 5.f, 6.f);

	while (true)
	{
		--tdata.id;
		tdata.position -= vec3(0.25, 0.1, 0.2);
		Log("ClientMainLoop.");
		//auto msg = gateway.PopInBox();
	
		auto m = std::make_shared<Network::TransformMsg>(tdata);
		gateway.AddToOutbox(0, m);	

		std::this_thread::sleep_for(std::chrono::milliseconds(400));
	}

	_getch();
	return 0;
	TestGameStart();
	return 0;
}
