#pragma once
#include <GraphicsApi/RendererType.h>

#include <unordered_map>

#include "LoD.h"
#include "SingleTon.h"
#include "Utils.h"
#include "ConfigurationParams/TerrainParam/TerrainParam.h"
#include "ConfigurationParams/DebugParams/PhysicsVisualizatorParams.h"

const std::string DEFAULT_DATA_PATH          = "../Data/";
const std::string DEFAULT_SHADERS_PATH       = "../Sources/";
const std::string DEFAULT_REQUIRED_FILE_NAME = "required_files.inf";

const vec2ui DEFAULT_WINDOW_SIZE = {1000, 600};

namespace GameEngine
{
namespace Params
{
struct Window
{
    vec2ui size      = DEFAULT_WINDOW_SIZE;
    bool fullScreen  = false;
    std::string name = "Default Window Name";
};
struct Sound
{
    bool isEnabled = true;
    float volume   = 1.f;
};
struct Files
{
    std::string data                    = DEFAULT_DATA_PATH;
    std::string shaders                 = DEFAULT_SHADERS_PATH;
    std::string requiredFilesOutputFile = DEFAULT_REQUIRED_FILE_NAME;
};

struct Shadows
{
    bool isEnabled = true;
    float distance = 35.f;
    uint32 mapSize = 4096;
};

struct Flora
{
    bool isEnabled     = true;
    bool isGrass       = true;
    float viewDistance = 10.f;
};

struct Particles
{
    bool useParticles = true;
};

enum class WaterType
{
    SIMPLE = 0,
    REFLECTED_REFRACTED,
    FULL
};

struct Water
{
    WaterType type                   = WaterType::FULL;
    vec2ui waterReflectionResolution = {640, 480};
    vec2ui waterRefractionResolution = {320, 240};
};

struct Textures
{
    vec2ui maxSize   = {1024, 1024};
    bool useAmbient  = true;
    bool useDiffuse  = true;
    bool useNormal   = true;
    bool useSpecular = true;
};

struct DebugParams
{
    PhysicsVisualizatorParams physicsVisualizator;
};

struct Renderer
{
    std::string graphicsApi        = "OpenGL";
    GraphicsApi::RendererType type = GraphicsApi::RendererType::FULL;

    float viewDistance             = 250.f;
    uint32 fpsLimt                 = 60;
    vec2ui resolution              = DEFAULT_WINDOW_SIZE;

    Terrain terrain;
    Water water;
    Flora flora;
    Shadows shadows;
    Textures textures;
    Particles particles;
};
}  // namespace Params

struct Configuration
{
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
