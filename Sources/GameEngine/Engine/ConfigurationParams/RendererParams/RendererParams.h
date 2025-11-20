#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include <Types.h>

#include "../ConfigurationParam.h"
#include "FloraParams/FloraParams.h"
#include "ParticlesParams/ParticlesParams.h"
#include "ShadowsParams/ShadowsParams.h"
#include "TerrainParam/TerrainParam.h"
#include "TexturesParams/TexturesParams.h"
#include "WaterParams/WaterParams.h"

namespace GameEngine
{
namespace Params
{
enum class PresetSettings
{
    UltraLow,
    Low,
    Medium,
    High,
    Ultra
};

std::string paramToString(PresetSettings);

struct Renderer
{
    Renderer();
    ConfigurationParam<std::string> graphicsApi;
    ConfigurationParam<GraphicsApi::RendererType> type;

    ConfigurationParam<PresetSettings> preset;
    ConfigurationParam<float> gamma;
    ConfigurationParam<float> exposure;
    ConfigurationParam<float> viewDistance;
    ConfigurationParam<float> normalMappingDistance;
    ConfigurationParam<uint32> fpsLimt;
    ConfigurationParam<vec2ui> resolution;
    ConfigurationParam<float> lodDistance0;
    ConfigurationParam<float> lodDistance1;
    ConfigurationParam<float> lodDistance2;
    ConfigurationParam<bool> useInstanceRendering;

    Terrain terrain;
    Water water;
    Flora flora;
    Shadows shadows;
    Textures textures;
    Particles particles;
};
}  // namespace Params
}  // namespace GameEngine
