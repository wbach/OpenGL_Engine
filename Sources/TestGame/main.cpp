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

std::vector<std::string> ExportAnimationClips(const GameEngine::File& file)
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
        return {};
    }

    std::cout << "Animation clips : " << std::to_string(model->animationClips_.size()) << std::endl;

    auto outputpath = std::filesystem::path(file.GetAbsoultePath() ).parent_path().string() + "/output/" + file.GetBaseName() + "_animationClips/";
    std::filesystem::create_directories(outputpath);

    auto maybeRootJoint = model->getRootJoint();
    std::vector<std::string> result;

    if (maybeRootJoint)
    {
        for (const auto& animation : model->animationClips_)
        {
            std::cout << "-- " << animation.first << std::endl;
            std::string outputFile = Utils::GetAbsolutePath(outputpath) + "/" + animation.first + ".xml";
            GameEngine::Animation::ExportAnimationClipToFile(outputFile, animation.second, *maybeRootJoint);
            result.push_back(outputFile);
        }
    }

    std::cout << file.GetBaseName() << " done." << std::endl;
    return result;
}

std::vector<std::string> exportAnimationClips(const GameEngine::File& input)
{
    if (input.GetExtension() != ".txt")
    {
        return ExportAnimationClips(input);
    }
    else
    {
        std::vector<std::string> result;
        for (const auto& file : Utils::ReadFileLines(input.GetAbsoultePath()))
        {
            auto exported = ExportAnimationClips(file);
            result.insert(result.end(), exported.begin(), exported.end());
        }
        return result;
    }
}

void glslInclude()
{
    std::string filename{"D:/tmp/tmp1/testShader.txt"};
    auto sourceCode = Utils::ReadFilesWithIncludes(filename);
    auto testFile   = std::filesystem::path(filename).stem().string();
    Utils::WrtieToFile("D:\\tmp\\" + testFile + "_all.txt", sourceCode);
}

void md5MultipleAnimsExtractor(const std::string& filename, const std::string& animationsDir)
{
    // ./TestGlGameExe -l -md5 /mnt/t610/GoogleDrive/GameData/Data/Meshes/DungeonDefenders/Spider/Spider_SK.md5mesh /mnt/t610/GoogleDrive/GameData/Data/Meshes/DungeonDefenders/Spider/Spider_Anims
    DEBUG_LOG("md5MultipleAnimsExtractor : " + filename + ", " + animationsDir);
    std::filesystem::path meshFile(filename);

    for (auto& p : std::filesystem::directory_iterator(std::filesystem::canonical(animationsDir)))
    {
        DEBUG_LOG(p.path().string() + ", extension : " + p.path().extension().string());
        //        if (p.is_directory())
        //        {
        //            if (p.path().string() != dir)
        //            {
        //                auto maybeFileName = FindFile(filename, p.path().string());
        //                if (not maybeFileName.empty())
        //                    return maybeFileName;
        //            }
        //        }
        //        else
        {
            if (p.path().extension() == ".md5anim")
            {
                std::string outputFile =
                    meshFile.parent_path().string() + "/" + meshFile.filename().stem().string() + ".md5anim";
                DEBUG_LOG(p.path().string() + " => " + outputFile);
                if (std::filesystem::exists(outputFile))
                {
                    std::filesystem::remove(outputFile);
                }

                std::filesystem::copy_file(p.path(), outputFile);

                auto exportedAnims = exportAnimationClips(filename);

                if (not exportedAnims.empty())
                {
                    std::filesystem::path outPath(exportedAnims.front());
                    std::string finalOutput =  outPath.parent_path().string() + "/"  + p.path().stem().string() + ".xml";
                    DEBUG_LOG(finalOutput);
                    Utils::RenameFile(exportedAnims.front(), finalOutput);
                }
            }
        }
    }
}

int main(int argc, char* argv[])
{
    std::cout << "Bengine Animation clips extractor." << std::endl;

    bool md5multipleConvert{false};
    for (int32 n = 1; n < argc; ++n)
    {
        if (std::string(argv[n]) == "-l")
        {
            CLogger::Instance().EnableLogs();
            CLogger::Instance().ImmeditalyLog();
            continue;
        }
        if (std::string(argv[n]) == "-md5")
        {
            md5multipleConvert = true;
            continue;
        }

        if (md5multipleConvert and n + 1 < argc)
        {
            md5MultipleAnimsExtractor(argv[n], argv[n + 1]);
            return 0;
        }
        else
        {
            exportAnimationClips(argv[n]);
        }
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
