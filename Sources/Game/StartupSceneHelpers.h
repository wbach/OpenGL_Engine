#pragma once

#include <Logger/Log.h>
#include <Utils/Json/JsonReader.h>

#include <filesystem>
#include <memory>
#include <string>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Scene/SceneFactoryBase.h"
#include "GameEngine/Scene/SceneUtils.h"
#include "TreeNode.h"

namespace Game::Startup
{
struct StartupSceneConfig
{
    std::unique_ptr<GameEngine::SceneFactoryBase> sceneFactory;
    std::string startupScene;
};

std::filesystem::path ResolveRuntimeProjectPath(const char* = nullptr);
StartupSceneConfig LoadStartupSceneConfig(const std::filesystem::path&);
void ApplyStartupScene(GameEngine::Engine&, const std::string&);
}  // namespace Game::Startup
