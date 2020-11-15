#include "../GameEngine/Engine/Engine.h"
#include "GameEngine/Api/Dummy/DummyGraphicsApi.h"
#ifndef USE_GNU
#include "DirectXApi/DirectXApi.h"
#endif
#include <Utils/FileSystem/FileSystemUtils.hpp>
#include "GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"
#include "MRpg/mrpg_main.h"
#include "OpenGLApi/OpenGLApi.h"
#include "Physics/PhyscisTestScene.h"
#include "Single/SingleStart.h"

#include <GameEngine/Resources/Models/WBLoader/LoaderManager.h>
#include <GameEngine/Resources/TextureLoader.h>
#include <GameEngine/Resources/GpuResourceLoader.h>
#include <GameEngine/Animations/AnimationUtils.h>
#include <GameEngine/Api/Dummy/DummyGraphicsApi.h>

void StartMessage()
{
    std::string stars = "********************************************\n";

    std::cout << stars
              << "Example usage Wojciech Bach game engine.\n"
                 "Run params :\n"
                 "-G  : load game(MMO, TEST)\n"
              << stars;
}

void PrintFiles()
{
    Utils::PrintFilesInDirectory(".");
}

void ExportAnimationClips(const GameEngine::File& file)
{
    GameEngine::DummyGraphicsApi api;
    GameEngine::GpuResourceLoader gpuLoader;
    GameEngine::TextureLoader textureLoader(api, gpuLoader);
    GameEngine::LoaderManager loader(textureLoader);

    auto model = loader.Load(file);

    DEBUG_LOG("Animation clips : " + std::to_string(model->animationClips_.size()));
    for (const auto& animation : model->animationClips_)
    {
        GameEngine::Animation::ExportAnimationClipToFile("D:/Tmp/" + animation.first + "_animationClip.xml", animation.second);
    }
}

int main(int argc, char* argv[])
{
    CLogger::Instance().EnableLogs();
    CLogger::Instance().ImmeditalyLog();

    ExportAnimationClips("D:/Projects/OpenGL_Engine/Data/Meshes/DaeAnimationExample/characeterIdle.dae");
    return 0;
    GameEngine::ReadFromFile("./Conf.xml");
    auto api = std::make_unique<OpenGLApi::OpenGLApi>();
    // GameEngine::ReadFromFile("./ConfDx11.xml"); auto api = std::make_unique<DirectX::DirectXApi>();

    StartMessage();


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
            // api = std::make_shared<GameEngine::DummyGraphicsApi>();
        }
    }

    if (!choosedApp)
        TestGame::TestGameStart(std::move(api));

    return 0;
}
