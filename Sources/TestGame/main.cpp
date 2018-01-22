#include "MainScene.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"
#include <fstream>
#include "FreeImage.h"
#include <conio.h>
#include "../UtilsNetwork/Gateway.h"
#include "../UtilsNetwork/Messages/SelectCharacter/SelectCharacterMsgReq.h"
#include "../UtilsNetwork/Messages/SelectCharacter/SelectCharacterMsgResp.h"

#include "Messages/TransformMessages/TransformMsg.h"

void TestGameStart()
{
	GameEngine::CEngine engine;
	engine.Init();
    engine.scene = std::make_unique<MainScene>(engine);
	engine.PreperaScene();
	engine.GameLoop();
}
const char serverAdressFilename[] = "./server.conf";
Network::CGateway gateway;

void OnMessageArrived(const Network::BoxMessage& mesage)
{
	Log("Receive SelectCharacterMsgResp.");

	auto msg = dynamic_cast<Network::SelectCharacterMsgResp*>(mesage.second.get());

	if (!msg) return;

	Log("SelectCharacterMsgResp : " + std::to_string(msg->id) +  ", status: " + std::to_string(msg->status_));
}

int main(int argc, char* argv[])
{	
	#ifdef BACHU_HAHAH
	std::cout << "DBACHU_HAHAH MINGW32 build." << std::endl;
	#endif
	CLogger::Instance().EnableLogs();
	
	std::string  serverAdress  = Utils::ReadFile(serverAdressFilename);
	Log("Server : " + serverAdress);

	//gateway.SubscribeOnMessageArrived(std::bind(&OnMessageArrived, std::placeholders::_1));
	
	gateway.ConnectToServer("baszek", "haslo", serverAdress, 1991);
	Network::SelectCharacterMsgReq characterSelectReq;
	characterSelectReq.id = 101;
	gateway.AddToOutbox(0, Network::CreateIMessagePtr<Network::SelectCharacterMsgReq>(characterSelectReq));
	Log("Wait for character select...");
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	Network::TransformMsg tdata;
	tdata.position = vec3(1.f, 2.f, 3.f);
	tdata.rotation = vec3(4.f, 5.f, 6.f);
	tdata.id = 101;
	tdata.type = Network::TransformMessageTypes::ROTATE_RIGHT;

	while (true)
	{
		//tdata.position -= vec3(0.25, 0.1, 0.2);
		Log("ClientMainLoop.");
		//auto msg = gateway.PopInBox();
	
		auto m = std::make_shared<Network::TransformMsg>(tdata);
		gateway.AddToOutbox(0, m);	

		gateway.PopInBox();

		std::this_thread::sleep_for(std::chrono::milliseconds(400));
	}

	_getch();
	return 0;
	TestGameStart();
	return 0;
}
