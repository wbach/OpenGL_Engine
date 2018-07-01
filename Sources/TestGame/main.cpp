#include "Single/SingleStart.h"
#include "MRpg/mrpg_main.h"
#include "../GameEngine/Engine/Engine.h"
#include "Logger/Log.h"
#include "GameEngine/Api/OpenGL/OpenGLApi.h"
#include "GameEngine/Api/Dummy/DummyGraphicsApi.h"
#include "GameEngine/Engine/Configuration.h"

void StartMessage()
{
	std::string stars = "********************************************\n";

	std::cout << stars <<
		"Example usage Wojciech Bach game engine.\n"
		"Run params :\n"
		"-G  : load game(MMO, TEST)\n"
	<< stars;
}

int main(int argc, char* argv[])
{
	GameEngine::ReadFromFile("./Conf.xml");

	GameEngine::IGraphicsApiPtr api = std::make_shared<GameEngine::OpenGLApi>();

 	StartMessage();

	CLogger::Instance().EnableLogs();

	bool choosedApp = false;

	for (int32 n = 1; n < argc; ++n)
	{
		std::string arg(argv[n]);
		if (arg == "-G")
		{
			if (n + 1 < argc)
			{
				++n;
				std::string gameName(argv[n]);

				if (gameName == "MMO")
				{
					MmmoRpg::StartGame(api);
					choosedApp = true;
				}
				if (gameName == "TEST")
				{
					TestGame::TestGameStart(api);
					choosedApp = true;
				}
			}
		}
		else if (arg == "-D")
		{
			api = std::make_shared<GameEngine::DummyGraphicsApi>();
		}
	}

	if(!choosedApp)
		TestGame::TestGameStart(api);

	return 0;
}
