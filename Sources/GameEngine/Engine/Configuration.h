#pragma once
#include "LoD.h"
#include "Utils.h"
#include "SingleTon.h"
#include <unordered_map>

const std::string DEFAULT_DATA_PATH = "../Data/";
const std::string DEFAULT_SHADERS_PATH = "../Shaders/";

struct SEngineConfiguration
{
    enum class RendererType
    {
        SIMPLE_RENDERER = 0,
        FULL_RENDERER
    };

    std::string GetFullDataPath(const std::string& file_name, bool addToRequierd = true);
    std::string GetFullShaderPath(const std::string& file_name, bool addToRequierd = true);
    std::string GetFilePatch(const std::string& file_full_path) const;
    void AddRequiredFile(const std::string& file);
    void SaveRequiredFiles();
    void ReadFromFile(const std::string& filename);

    std::string dataFilesLocation    = DEFAULT_DATA_PATH;
    std::string shadersFilesLocation = DEFAULT_SHADERS_PATH;

    std::vector<std::string> requiredFiles;

    // Window
    std::string windowName = "Default Window Name";
    vec2ui resolution   = {1000, 600};
    vec2ui windowSize   = {1000, 600};

    bool fullScreen	 = false;
    float refresRate = 60;
	bool vsync		 = true;

    // Sound
    bool sound        = true;
    float soundVolume = 1.f;

    // Water
    bool isHighWaterQuality             = true;
    vec2i waterReflectionResolution = {640, 480};
    vec2i waterRefractionResolution = {320, 240};

    // Models
    vec2i maxTextureResolutuion = {128, 128};

    // Shadows
    bool isShadows        = true;
    float shadowsDistance = 200.f;
    int shadowMapSize     = 1024;

    // ViewDistance
    float viewDistance = 200.f;

    // flora
    float floraViewDistance = 100.f;
    bool advancedGrass      = true;

	//Effects
	bool useParticles = true;

    // Renderer
    RendererType rendererType = RendererType::FULL_RENDERER;  // simple renderer/full renderer
    LoD startLod;
    // float       m_RenderingResolution = 1.f;

	// Resources
	bool enableBinaryLoading = true;

    std::string requiredFilesOutputFile = "required_files.inf";

    std::unordered_map<std::string,int> texturesIds;// dirty hack, debug
};

#define EngineConf SingleTon<SEngineConfiguration>::Get()
#define EngineConf_SaveRequiredFiles() EngineConf.SaveRequiredFiles()
#define EngineConf_GetFullDataPath(x) EngineConf.GetFullDataPath(x, false)
#define EngineConf_GetFullDataPathAddToRequierd(x) EngineConf.GetFullDataPath(x)
#define EngineConf_GetFullShaderPath(x) EngineConf.GetFullShaderPath(x, false)
#define EngineConf_GetFullShaderPathAddToRequierd(x) EngineConf.GetFullShaderPath(x)
#define EngineConf_GetOrginFilePath(x) EngineConf.GetFilePatch(x)
#define EngineConf_AddRequiredFile(x) EngineConf.AddRequiredFile(x)
