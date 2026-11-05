#include "StartupSceneHelpers.h"

namespace Game::Startup
{
namespace
{
// inline const char GAME_FOLDER_NAME[] = "game";
}

using namespace GameEngine::Params;

std::filesystem::path ResolveCurrentPath(const char* argv0)
{
    auto projectDir = std::filesystem::current_path();

    if (argv0 != nullptr && argv0[0] != '\0')
    {
        const auto executablePath = std::filesystem::absolute(std::filesystem::path{argv0});
        if (executablePath.has_parent_path())
        {
            const auto executableDir = executablePath.parent_path();
            if (std::filesystem::exists(executableDir / "scenes.json") || std::filesystem::exists(executableDir / "workSpaceConfig.xml"))
            {
                LOG_INFO << "Project path: " << executableDir;
                return executableDir;
            }
        }
    }

    LOG_INFO << "Project path: " << projectDir;
    if (std::filesystem::exists(projectDir / "scenes.json") || std::filesystem::exists(projectDir / "workSpaceConfig.xml"))
    {
        return projectDir;
    }

    return projectDir;
}

std::filesystem::path ResolveRuntimeProjectPath(const char* argv0)
{
    auto currentPath = ResolveCurrentPath(argv0);

    if (std::filesystem::exists(currentPath))
    {
        auto projectPath = std::filesystem::absolute(currentPath / "game").lexically_normal();

        auto projectLocalConfigPath = std::filesystem::absolute(projectPath / "workSpaceConfig.xml").lexically_normal();
        GameEngine::ReadConfigFromFile(EngineLocalConf, projectLocalConfigPath);
        EngineLocalConf.files.setProjectPath(projectPath);

        if (not std::filesystem::exists(EngineLocalConf.files.getShaderPath()))
        {
            LOG_WARN << "Shader path not found: " << EngineLocalConf.files.getShaderPath() << ". Using default shader path.";
            EngineLocalConf.files.setShaderPath(projectPath / "Shaders");
        }

        LOG_INFO << "Runtime project path: " << projectPath;
        return projectPath;
    }

    LOG_ERROR << "Project path not found: " << currentPath;
    return {};
}

StartupSceneConfig LoadStartupSceneConfig(const std::filesystem::path& projectPath)
{
    StartupSceneConfig config{};
    const auto scenesFilePath = std::filesystem::absolute(projectPath / "scenes.json");

    Utils::JsonReader jsonReader;
    if (!jsonReader.Read(scenesFilePath))
    {
        if (std::filesystem::exists(scenesFilePath))
        {
            LOG_ERROR << "Scene config read failed: " << scenesFilePath;
            return config;
        }

        GameEngine::createScenesFile(GameEngine::File{scenesFilePath});
        LOG_INFO << "Created default scenes config: " << scenesFilePath;
        return config;
    }

    const auto scenesNode = jsonReader.Get("scenes");
    PrintTree(*jsonReader.Get());

    if (!scenesNode)
    {
        LOG_ERROR << "scenes not found in project config file";
        return config;
    }

    config.sceneFactory = std::make_unique<GameEngine::SceneFactoryBase>();

    for (const auto& sceneNode : scenesNode->getChildren())
    {
        if (!sceneNode)
            continue;

        LOG_DEBUG << "Add scene " << sceneNode->name() << ", " << sceneNode->value_;
        config.sceneFactory->AddScene(
            sceneNode->name(), GameEngine::File{std::filesystem::absolute(projectPath / sceneNode->value_).lexically_normal()});
    }

    if (config.sceneFactory->ScenesSize() < 1)
    {
        LOG_ERROR << "Scene not available";
        config.sceneFactory.reset();
        return config;
    }

    const auto startupScene = jsonReader.Get("startupScene");
    if (startupScene)
    {
        config.startupScene = startupScene->value_;
    }

    return config;
}
void ApplyStartupScene(GameEngine::Engine& engine, const std::string& startupScene)
{
    if (startupScene.empty())
        return;

    LOG_DEBUG << "Startup scene " << startupScene;
    engine.GetSceneManager().SetActiveScene(startupScene);
}
}  // namespace Game::Startup