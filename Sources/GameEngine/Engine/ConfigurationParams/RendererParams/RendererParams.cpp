#include "RendererParams.h"

#include "GameEngine/Engine/ConfigurationParams/PramsConstants.h"

namespace GameEngine
{
namespace Params
{
#define str(x)              \
    case PresetSettings::x: \
        return #x
std::string paramToString(PresetSettings preset)
{
    switch (preset)
    {
        str(UltraLow);
        str(Low);
        str(Medium);
        str(High);
        str(Ultra);
    }
    return "undefined";
}
#undef str

namespace
{
std::vector<std::string> avaiablesApis()
{
    std::vector<std::string> result{{"OpenGL"}};
#ifndef USE_GNU
#ifndef USE_MINGW  // TO DO
    result.push_back({"DirectX11"});
#endif
#endif
    return result;
}
}  // namespace
Renderer::Renderer()
    : graphicsApi{{"OpenGL"}, avaiablesApis()}
    , type{GraphicsApi::RendererType::FULL}
    , preset{PresetSettings::Ultra,
             {PresetSettings::UltraLow, PresetSettings::Low, PresetSettings::Medium, PresetSettings::High, PresetSettings::Ultra}}
    , gamma{1.6f, {1.0f, 1.2f, 1.4f, 1.6f, 1.8f, 2.0f, 2.2f, 2.4f, 2.6f, 2.8f, 3.0f}}
    , exposure{0.7f, {0.05f, 0.07f, 0.09f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.f}}
    , viewDistance{2000.f, {50.f, 100.f, 200.f, 400.f, 800.f, 1000.f, 1200.f, 1500.f, 2000.f, 3000.f, 4000.f, 5000.f, 10000.f}}
    , normalMappingDistance{200.f, {100.f, 200.f, 400.f}}
    , fpsLimt{60, {0, 30, 60, 75, 144}}
    , resolution{DEFAULT_WINDOW_SIZE, defaultResolutions()}
    , lodDistance0{50.f, {0.f, 25.f, 50.f, 75.f, 100.f}}
    , lodDistance1{100.f, {25.f, 50.f, 75.f, 100.f, 125.f, 150.f, 175.f, 200.f}}
    , lodDistance2{150.f, {25.f, 50.f, 75.f, 100.f, 125.f, 150.f, 175.f, 200.f}}
    , useInstanceRendering{true, {false, true}}
{
    preset.subscribeForChange(
        [this]()
        {
            switch (preset)
            {
                case PresetSettings::UltraLow:
                    type                            = GraphicsApi::RendererType::SIMPLE;
                    useInstanceRendering            = false;
                    viewDistance                    = 100.f;
                    lodDistance0                    = 0;
                    lodDistance1                    = 25.f;
                    textures.limitTextureSize       = true;
                    textures.maxSize                = vec2ui(128, 128);
                    textures.useNormal              = false;
                    flora.isEnabled                 = false;
                    flora.viewDistance              = 0.f;
                    normalMappingDistance           = 0.f;
                    particles.useParticles          = false;
                    water.type                      = GameEngine::Params::WaterType::SIMPLE;
                    water.waterReflectionResolution = vec2ui(16, 16);
                    water.waterRefractionResolution = vec2ui(16, 16);
                    shadows.isEnabled               = false;
                    shadows.distance                = 0.f;
                    shadows.mapSize                 = 16;
                    shadows.cascadesSize            = 1;
                    terrain.resolutionDivideFactor  = 2;
                    terrain.meshPartsCount          = 8;
                    break;
                case PresetSettings::Low:
                    type                            = GraphicsApi::RendererType::SIMPLE;
                    useInstanceRendering            = false;
                    viewDistance                    = 200.f;
                    lodDistance0                    = 25.f;
                    lodDistance1                    = 50.f;
                    textures.limitTextureSize       = true;
                    textures.maxSize                = vec2ui(512, 512);
                    textures.useNormal              = false;
                    flora.isEnabled                 = false;
                    flora.viewDistance              = 0.f;
                    normalMappingDistance           = 0.f;
                    particles.useParticles          = true;
                    water.type                      = GameEngine::Params::WaterType::SIMPLE;
                    water.waterReflectionResolution = vec2ui(16, 16);
                    water.waterRefractionResolution = vec2ui(16, 16);
                    shadows.isEnabled               = false;
                    shadows.distance                = 0.f;
                    shadows.mapSize                 = 16;
                    shadows.cascadesSize            = 1;
                    terrain.resolutionDivideFactor  = 1;
                    terrain.meshPartsCount          = 8;
                    break;
                case PresetSettings::Medium:
                    type                            = GraphicsApi::RendererType::FULL;
                    useInstanceRendering            = false;
                    viewDistance                    = 800.f;
                    lodDistance0                    = 50.f;
                    lodDistance1                    = 100.f;
                    textures.limitTextureSize       = true;
                    textures.maxSize                = vec2ui(1024, 1024);
                    flora.isEnabled                 = false;
                    flora.viewDistance              = 0.f;
                    textures.useNormal              = true;
                    normalMappingDistance           = 10.f;
                    particles.useParticles          = true;
                    water.type                      = GameEngine::Params::WaterType::SIMPLE;
                    water.waterReflectionResolution = vec2ui(16, 16);
                    water.waterRefractionResolution = vec2ui(16, 16);
                    shadows.isEnabled               = true;
                    shadows.distance                = 25.f;
                    shadows.firstCascadeDistance    = 5.f;
                    shadows.mapSize                 = 2048;
                    shadows.cascadesSize            = 4;
                    terrain.resolutionDivideFactor  = 1;
                    terrain.meshPartsCount          = 8;
                    break;
                case PresetSettings::High:
                    type                            = GraphicsApi::RendererType::FULL;
                    useInstanceRendering            = false;
                    viewDistance                    = 1500.f;
                    lodDistance0                    = 100.f;
                    lodDistance1                    = 200.f;
                    textures.limitTextureSize       = true;
                    textures.maxSize                = vec2ui(2048, 2048);
                    flora.isEnabled                 = true;
                    flora.viewDistance              = 50.f;
                    textures.useNormal              = true;
                    normalMappingDistance           = 50.f;
                    particles.useParticles          = true;
                    water.type                      = GameEngine::Params::WaterType::REFLECTED_REFRACTED;
                    water.waterReflectionResolution = vec2ui(1366, 768);
                    water.waterRefractionResolution = vec2ui(1366, 768);
                    shadows.isEnabled               = true;
                    shadows.distance                = 50.f;
                    shadows.firstCascadeDistance    = 13.f;
                    shadows.mapSize                 = 4096;
                    shadows.cascadesSize            = 4;
                    terrain.resolutionDivideFactor  = 1;
                    terrain.meshPartsCount          = 8;
                    break;
                case PresetSettings::Ultra:
                    type                            = GraphicsApi::RendererType::FULL;
                    useInstanceRendering            = false;
                    viewDistance                    = 2000.f;
                    lodDistance0                    = 200.f;
                    lodDistance1                    = 300.f;
                    textures.limitTextureSize       = false;
                    textures.maxSize                = vec2ui(8192, 8192);
                    flora.isEnabled                 = true;
                    flora.viewDistance              = 100.f;
                    textures.useNormal              = true;
                    normalMappingDistance           = 50.f;
                    particles.useParticles          = true;
                    water.type                      = GameEngine::Params::WaterType::REFLECTED_REFRACTED;
                    water.waterReflectionResolution = vec2ui(1920, 1080);
                    water.waterRefractionResolution = vec2ui(1920, 1080);
                    shadows.isEnabled               = true;
                    shadows.distance                = 100.f;
                    shadows.firstCascadeDistance    = 21.f;
                    shadows.mapSize                 = 8192;
                    shadows.cascadesSize            = 4;
                    terrain.resolutionDivideFactor  = 1;
                    terrain.meshPartsCount          = 8;
                    break;
            }
        });
}
}  // namespace Params
}  // namespace GameEngine
