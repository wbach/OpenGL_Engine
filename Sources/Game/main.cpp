#include <Logger/Log.h>
#include <Utils/Json/JsonReader.h>
#include <Utils/Json/JsonWriter.h>

#include <filesystem>
#include <iostream>
#include <string>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Scene/SceneFactoryBase.h"
#include "GameEngine/Scene/SceneUtils.h"
#include "Logger/Log.h"
#include "TreeNode.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

namespace
{
const std::string CSTR_ROOT_NODE{"projectConfiguration"};
const std::string CSTR_STARTUP_SCENE_NODE{"startupScene"};
const std::string CSTR_SCENES_NODE{"scenes"};
}  // namespace

void Start()
{
    CLogger::Instance().EnableLogs();
    CLogger::Instance().UseAsyncLogging(false);

    Utils::JsonReader jsonReader;
    const auto& inputFile = EngineConf.files.getDataPath() / "Projects" / "avatar.json";
    if (not jsonReader.Read(inputFile))
    {
        LOG_DEBUG << inputFile << " read failed";
        if (std::filesystem::exists(inputFile))
            return;

        GameEngine::createScenesFile(inputFile);
        return;
    }

    auto scenesNode = jsonReader.Get(CSTR_SCENES_NODE);
    PrintTree(*jsonReader.Get());
    if (not scenesNode)
    {
        LOG_DEBUG << CSTR_SCENES_NODE << " not found in project config file";
    }

    auto sceneFactory = std::make_unique<SceneFactoryBase>();
    for (const auto& sceneNode : scenesNode->getChildren())
    {
        if (not sceneNode)
            continue;

        LOG_DEBUG << "Add scene " << sceneNode->name() << ", " << sceneNode->value_;
        sceneFactory->AddScene(sceneNode->name(), GameEngine::File{sceneNode->value_});
    }

    if (sceneFactory->ScenesSize() < 1)
    {
        LOG_ERROR << "Scene not avaiable";
        return;
    }

    Engine engine(std::make_unique<Bullet::BulletAdapter>(), std::move(sceneFactory));
    engine.Init();

    auto startupScene = jsonReader.Get(CSTR_STARTUP_SCENE_NODE);
    if (startupScene)
    {
        LOG_DEBUG << "Startup scene " << startupScene->value_;
        engine.GetSceneManager().SetActiveScene(startupScene->value_);
    }
    // if (not engine.GetSceneManager().GetActiveScene())
    // {
    //     engine.GetSceneManager().SetActiveScene(0);
    // }
    engine.GameLoop();
}

int main(int, char**)
{
    std::cout << "The game starter\n";
    Start();
    return 0;
}
