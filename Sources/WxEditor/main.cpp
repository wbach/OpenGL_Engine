#include <GameEngine/Engine/Engine.h>
#include <GameEngine/Physics/Bullet/BulletAdapter.h>
#include <GameEngine/Scene/SceneFactoryBase.h>
#include <wx/log.h>

#include "App.h"
#include "Logger/Log.h"
#include "Utils.h"
#include "WxEditor/ProjectManager.h"

wxIMPLEMENT_APP_NO_MAIN(App);

using namespace GameEngine;
using namespace GameEngine::Physics;

class SceneFactory : public GameEngine::SceneFactoryBase
{
public:
    SceneFactory(const GameEngine::File& file)
    {
        AddScene(file.GetBaseName(), file);
    }
};

int main(int argc, char* argv[])
{
    auto args = Utils::parseArguments(argc, argv);

    if (auto path = Utils::GetValue(args, "projectPath"))
    {
        auto& pm = ProjectManager::GetInstance();
        pm.SetProjectPath(*path);
    }

    if (auto value = Utils::GetValue(args, "scene"))
    {
        File file{*value};
        if (file.exist())
        {
            CLogger::Instance().SetLogFilename("WxEdtiorGameRunnerLogs.txt");
            CLogger::Instance().EnableLogs();
            std::cout << "Starting game engine with scene: " << file << "\n";
            Engine engine(std::make_unique<Bullet::BulletAdapter>(), std::make_unique<SceneFactory>(file));
            engine.Init();
            engine.GetSceneManager().SetActiveScene(file.GetBaseName());
            engine.GameLoop();
            return 0;
        }

        LOG_ERROR << "Starting scene failed. " << *value;

        return 0;
    }

    return wxEntry(argc, argv);
}
