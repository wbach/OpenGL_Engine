#include "../GameEngine/Engine/Engine.h"
#include "GameEngine/Api/Dummy/DummyGraphicsApi.h"
#ifndef USE_GNU
#include "DirectXApi/DirectXApi.h"
#endif
#include <GameEngine/Animations/AnimationUtils.h>
#include <GameEngine/Api/Dummy/DummyGraphicsApi.h>
#include <GameEngine/Resources/GpuResourceLoader.h>
#include <GameEngine/Resources/Models/WBLoader/LoaderManager.h>
#include <GameEngine/Resources/TextureLoader.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"
#include "MRpg/mrpg_main.h"
#include "OpenGLApi/OpenGLApi.h"
#include "Physics/PhyscisTestScene.h"
#include "Single/SingleStart.h"

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
    std::cout << file.GetAbsoultePath() << std::endl;

    GameEngine::DummyGraphicsApi api;
    GameEngine::GpuResourceLoader gpuLoader;
    GameEngine::TextureLoader textureLoader(api, gpuLoader);
    GameEngine::LoaderManager loader(textureLoader);

    auto model = loader.Load(file);

    if (not model)
    {
        std::cout << file.GetBaseName() << " create model error. " << file.GetAbsoultePath() << std::endl;
        return;
    }

    std::cout << "Animation clips : " << std::to_string(model->animationClips_.size()) << std::endl;

    auto outputpath = "./output/" + file.GetBaseName() + "_animationClips/";
    std::filesystem::create_directories(outputpath);

    auto maybeRootJoint = model->getRootJoint();

    if (maybeRootJoint)
    {
        for (const auto& animation : model->animationClips_)
        {
            std::cout << "-- " << animation.first << std::endl;
            GameEngine::Animation::ExportAnimationClipToFile(
                Utils::GetAbsolutePath(outputpath) + "/" + animation.first + ".xml", animation.second, *maybeRootJoint);
        }
    }

    std::cout << file.GetBaseName() << " done." << std::endl;
}

void exportAnimationClips(const GameEngine::File& input)
{
    if (input.GetExtension() != ".txt")
    {
        ExportAnimationClips(input);
    }
    else
    {
        for (const auto& file : Utils::ReadFileLines(input.GetAbsoultePath()))
        {
            ExportAnimationClips(file);
        }
    }
}

void glslInclude()
{
    std::string filename{"D:/tmp/tmp1/testShader.txt"};
    auto sourceCode = Utils::ReadFilesWithIncludes(filename);
    auto testFile   = std::filesystem::path(filename).stem().string();
    Utils::WrtieToFile("D:\\tmp\\" + testFile + "_all.txt", sourceCode);
}

int main(int argc, char* argv[])
{
    std::cout << "Bengine Animation clips extractor." << std::endl;

    for (int32 n = 1; n < argc; ++n)
    {
        if (std::string(argv[n]) == "-l")
        {
            CLogger::Instance().EnableLogs();
            CLogger::Instance().ImmeditalyLog();
            continue;
        }

        exportAnimationClips(argv[n]);
    }
}
//
// int main(int argc, char* argv[])
//{
//    CLogger::Instance().EnableLogs();
//    CLogger::Instance().ImmeditalyLog();
//
//    // exportAnimationClips(); return 0;
//    glslInclude();
//    return 0;
//
//    GameEngine::ReadFromFile("./Conf.xml");
//    auto api = std::make_unique<OpenGLApi::OpenGLApi>();
//    // GameEngine::ReadFromFile("./ConfDx11.xml"); auto api = std::make_unique<DirectX::DirectXApi>();
//
//    StartMessage();
//
//    bool choosedApp = false;
//
//    for (int32 n = 1; n < argc; ++n)
//    {
//        std::string arg(argv[n]);
//        if (arg == "-G")
//        {
//            if (n + 1 < argc)
//            {
//                ++n;
//                std::string gameName(argv[n]);
//
//                if (gameName == "MMO")
//                {
//                    MmmoRpg::StartGame(std::move(api));
//                    choosedApp = true;
//                }
//                if (gameName == "TEST")
//                {
//                    TestGame::TestGameStart(std::move(api));
//                    choosedApp = true;
//                }
//                if (gameName == "PHYSICS")
//                {
//                    PhysicsTestGame::Start(std::move(api));
//                    choosedApp = true;
//                }
//            }
//        }
//        else if (arg == "-D")
//        {
//            // api = std::make_shared<GameEngine::DummyGraphicsApi>();
//        }
//    }
//
//    if (!choosedApp)
//        TestGame::TestGameStart(std::move(api));
//
//    return 0;
//}
