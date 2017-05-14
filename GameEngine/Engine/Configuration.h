#pragma once
#include "../Utils/Utils.h"
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

	void ReadFromFile(const std::string& filename);

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
private:
	SConfiguration() {}
	SConfiguration(const SConfiguration &) {}
};
