#include "../GameEngine/Engine/Engine.h"
#include "GameEngine/Api/Dummy/DummyGraphicsApi.h"
#include "DirectXApi/DirectXApi.h"
#include "OpenGLApi/OpenGLApi.h"
#include "GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"
#include "MRpg/mrpg_main.h"
#include "Physics/PhyscisTestScene.h"
#include "Single/SingleStart.h"

void StartMessage()
{
    std::string stars = "********************************************\n";

    std::cout << stars << "Example usage Wojciech Bach game engine.\n"
                          "Run params :\n"
                          "-G  : load game(MMO, TEST)\n"
              << stars;
}

int main(int argc, char* argv[])
{
    GameEngine::ReadFromFile("./Conf.xml");

    //auto api = std::make_unique<OpenGLApi::OpenGLApi>();
    auto api = std::make_unique<DirectX::DirectXApi>();

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
                    MmmoRpg::StartGame(std::move(api));
                    choosedApp = true;
                }
                if (gameName == "TEST")
                {
                    TestGame::TestGameStart(std::move(api));
                    choosedApp = true;
                }
                if (gameName == "PHYSICS")
                {
                    PhysicsTestGame::Start(std::move(api));
                    choosedApp = true;
                }
            }
        }
        else if (arg == "-D")
        {
            //api = std::make_shared<GameEngine::DummyGraphicsApi>();
        }
    }

    if (!choosedApp)
        TestGame::TestGameStart(std::move(api));

    return 0;
}
