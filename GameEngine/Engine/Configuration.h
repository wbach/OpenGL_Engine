#pragma once
#include "Utils.h"
#include "LoD.h"

class SConfiguration
{
public:
    enum class RendererType
    {
        SIMPLE_RENDERER = 0,
        FULL_RENDERER
    };

	static SConfiguration& Instance();
	std::string GetFullDataPath(const std::string& file_name, bool addToRequierd = true);
	std::string GetFullShaderPath(const std::string& file_name, bool addToRequierd = true);
	std::string GetFilePatch(const std::string& file_full_path) const;
	void AddRequiredFile(const std::string& file);
	void SaveRequiredFiles();
	void ReadFromFile(const std::string& filename);

    std::string dataFilesLocation = "../Data/";
    std::string shadersFilesLocation = "../Shaders/";

	std::vector<std::string> requiredFiles;

    //Window
    std::string windowName = "Default Window Name";
    wb::vec2i   resolution = {1000, 600};
    bool        fullScreen = false;
    float       refresRate = 60;

    //Sound
    bool        sound         = true;
    float       soundVolume   = 1.f;

    //Water
    bool        isHighWaterQuality = true;
    wb::vec2i   waterReflectionResolution = {640, 480};
    wb::vec2i   waterRefractionResolution = {320, 240};

    //Models
    wb::vec2i   maxTextureResolutuion = {4096, 4096};

    //Shadows
    bool        isShadows = true;
    float       shadowsDistance = 200.f;
    int         shadowMapSize = 4096;

    //ViewDistance
    float       viewDistance = 200.f;

	//flora
    float       floraViewDistance = 100.f;
	bool		advancedGrass = true;

    //Renderer
	RendererType rendererType = RendererType::FULL_RENDERER;   //simple renderer/full renderer
	LoD			 startLod;
    //float       m_RenderingResolution = 1.f;

	std::string requiredFilesOutputFile = "required_files.inf";

private:
	SConfiguration() {}
	SConfiguration(const SConfiguration &) {}
};

#define EngineConf SConfiguration::Instance()
#define EngineConf_SaveRequiredFiles() SConfiguration::Instance().SaveRequiredFiles()
#define EngineConf_GetFullDataPath(x) SConfiguration::Instance().GetFullDataPath(x, false)
#define EngineConf_GetFullDataPathAddToRequierd(x) SConfiguration::Instance().GetFullDataPath(x)
#define EngineConf_GetFullShaderPath(x) SConfiguration::Instance().GetFullShaderPath(x, false)
#define EngineConf_GetFullShaderPathAddToRequierd(x) SConfiguration::Instance().GetFullShaderPath(x)
#define EngineConf_GetOrginFilePath(x) SConfiguration::Instance().GetFilePatch(x)
#define EngineConf_AddRequiredFile(x) SConfiguration::Instance().AddRequiredFile(x)