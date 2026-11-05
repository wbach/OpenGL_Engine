#ifndef __EMSCRIPTEN__
#include <Logger/Log.h>

#include <filesystem>
#include <iostream>
#include <string>

#include "Game/StartupSceneHelpers.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

void Start(const char* executablePath)
{
    CLogger::Instance().EnableLogs();
    CLogger::Instance().UseAsyncLogging(false);

    const auto projectPath      = Game::Startup::ResolveRuntimeProjectPath(executablePath);

    auto startupConfig = Game::Startup::LoadStartupSceneConfig(projectPath);
    if (!startupConfig.sceneFactory)
    {
        return;
    }

    Engine engine(std::make_unique<Bullet::BulletAdapter>(), std::move(startupConfig.sceneFactory));
    engine.Init();
    Game::Startup::ApplyStartupScene(engine, startupConfig.startupScene);
    engine.GameLoop();
}

int main(int argc, char** argv)
{
    std::cout << "The game starter\n";
    Start(argv[0]);
    (void)argc;
    return 0;
}
#endif