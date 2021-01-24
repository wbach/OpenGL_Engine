#pragma once
#include <GraphicsApi/RendererType.h>
#include <SingleTon.h>
#include <Utils.h>

#include <unordered_map>

#include "ConfigurationParams/ConfigurationParam.h"
#include "ConfigurationParams/DebugParams/PhysicsVisualizatorParams.h"
#include "ConfigurationParams/RendererParams/RendererParams.h"
#include "ConfigurationParams/SoundParams/SoundParams.h"
#include "ConfigurationParams/WindowParams/WindowParams.h"
#include "LoD.h"
#include <Logger/Log.h>

const std::string DEFAULT_DATA_PATH          = "../Data/";
const std::string DEFAULT_SHADERS_PATH       = "../Sources/";
const std::string DEFAULT_REQUIRED_FILE_NAME = "required_files.inf";

namespace GameEngine
{
namespace Params
{
struct Files
{
    std::string data                    = DEFAULT_DATA_PATH;
    std::string shaders                 = DEFAULT_SHADERS_PATH;
    std::string requiredFilesOutputFile = DEFAULT_REQUIRED_FILE_NAME;
};

struct DebugParams
{
    DebugParams();
    PhysicsVisualizatorParams physicsVisualizator;
    Params::ConfigurationParam<LogginLvl> logLvl;
    Params::ConfigurationParam<bool> showRenderInfo;
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
void AddRequiredFile(const std::string& file);
void ReadFromFile(const std::string& filename);
std::string RemoveDataPath(const std::string&);
std::string GetFullDataPath(const std::string& file_name, bool addToRequierd = true);
std::string GetFullShaderPath(const std::string& file_name, bool addToRequierd = true);
std::string GetFilePatch(const std::string& file_full_path);
std::string GetDataLocationFromString(const std::string& str);
std::string GetShaderLocationFromString(const std::string& str);
std::string GetRelativeDataPath(const std::string&);
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
