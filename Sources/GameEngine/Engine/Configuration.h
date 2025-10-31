#pragma once
#include <GraphicsApi/RendererType.h>
#include <Logger/LoggingLvl.h>
#include <SingleTon.h>
#include <Utils.h>

#include <unordered_map>

#include "ConfigurationParams/ConfigurationParam.h"
#include "ConfigurationParams/DebugParams/LineMeshVisualizatorParams.h"
#include "ConfigurationParams/RendererParams/RendererParams.h"
#include "ConfigurationParams/SoundParams/SoundParams.h"
#include "ConfigurationParams/WindowParams/WindowParams.h"
#include "LoD.h"

const std::filesystem::path DEFAULT_DATA_PATH          = "../Data/";
const std::filesystem::path DEFAULT_SHADERS_PATH       = "../Sources/";
const std::filesystem::path DEFAULT_CACHE_PATH         = "../Cache/";
const std::filesystem::path DEFAULT_USER_DIR_PATH      = "../User/";
const std::filesystem::path DEFAULT_REQUIRED_FILE_NAME = "required_files.inf";

namespace GameEngine
{
namespace Params
{
struct Files
{
    std::filesystem::path data                    = DEFAULT_DATA_PATH;
    std::filesystem::path shaders                 = DEFAULT_SHADERS_PATH;
    std::filesystem::path cache                   = DEFAULT_CACHE_PATH;
    std::filesystem::path userDir                 = DEFAULT_USER_DIR_PATH;
    std::filesystem::path requiredFilesOutputFile = DEFAULT_REQUIRED_FILE_NAME;
    std::filesystem::path loadingScreenCircleTexture{"GUI/aLoadingCircle.png"};
    std::filesystem::path loadingScreenBackgroundTexture{"GUI/dark3.png"};
};

struct DebugParams
{
    DebugParams();
    LineMeshVisualizatorParams linemeshVisualizator;
    Params::ConfigurationParam<LoggingLvl> logLvl;
    Params::ConfigurationParam<bool> showRenderInfo;
    Params::ConfigurationParam<bool> showPhycicsVisualization;
};
}  // namespace Params

struct Configuration
{
    Configuration();
    std::string filename;
    Params::Files files;
    Params::Sound sound;
    Params::Window window;
    Params::Renderer renderer;
    Params::DebugParams debugParams;
    bool useBinaryLoading = false;
};

void SaveRequiredFiles();
void AddRequiredFile(const std::filesystem::path&);
void ReadFromFile(const std::filesystem::path&);
void CreateDefaultFile(const std::filesystem::path&);
std::filesystem::path RemoveDataPath(const std::filesystem::path&);
std::filesystem::path GetFullDataPath(const std::filesystem::path&, bool addToRequierd = true);
std::filesystem::path GetFullShaderPath(const std::filesystem::path&, bool addToRequierd = true);
std::filesystem::path GetFilePatch(const std::filesystem::path&);
std::filesystem::path GetDataLocationFromString(const std::string&);
std::filesystem::path GetCacheLocationFromString(const std::string&);
std::filesystem::path GetShaderLocationFromString(const std::string&);
std::filesystem::path GetRelativeDataPath(const std::filesystem::path&);
}  // namespace GameEngine

#define EngineConf SingleTon<GameEngine::Configuration>::Get()
#define EngineConf_SaveRequiredFiles() GameEngine::SaveRequiredFiles()
#define EngineConf_GetFullDataPath(x) GameEngine::GetFullDataPath(x, false)
#define EngineConf_GetFullDataPathAddToRequierd(x) GameEngine::GetFullDataPath(x)
#define EngineConf_GetFullShaderPath(x) GameEngine::GetFullShaderPath(x, false)
#define EngineConf_GetFullShaderPathAddToRequierd(x) GameEngine::GetFullShaderPath(x)
#define EngineConf_GetOrginFilePath(x) GameEngine::GetFilePatch(x)
#define EngineConf_AddRequiredFile(x) GameEngine::AddRequiredFile(x)
#define EngineConf_RemoveDataPath(x) GameEngine::RemoveDataPath(x)
