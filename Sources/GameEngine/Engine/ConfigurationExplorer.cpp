#include "ConfigurationExplorer.h"

#include <algorithm>

#include "Configuration.h"

namespace GameEngine
{
namespace
{
ConfigurationExplorer::Params emptyParams;
}  // namespace

// clang-format off
ConfigurationExplorer::ConfigurationExplorer()
{
    categories.push_back({"Display",
                          {
                              {"Api", EngineConf.renderer.graphicsApi, ApplyPolicy::RestartRequired},
                              {"Fullscreen mode", EngineConf.window.fullScreen, ApplyPolicy::RestartNotNeeded},
                              {"Window size", EngineConf.window.size, ApplyPolicy::RestartNotNeeded},
                              {"Render size", EngineConf.renderer.resolution, ApplyPolicy::RestartRequired},
                              {"Fps limit", EngineConf.renderer.fpsLimt, ApplyPolicy::RestartNotNeeded}
                          }
                        });
    categories.push_back(
        {"Graphics",
         {
             {"Preset settings", EngineConf.renderer.preset, ApplyPolicy::RestartRequired, ParamsImpact::HasImpact},
             {"Renderer type", EngineConf.renderer.type, ApplyPolicy::RestartRequired},
             //{"UseInstanceRendering", EngineConf.renderer.useInstanceRendering, ApplyPolicy::RestartNotNeeded},
             {"View distance", EngineConf.renderer.viewDistance, ApplyPolicy::RestartNotNeeded},
             {"Exposure", EngineConf.renderer.exposure, ApplyPolicy::RestartNotNeeded},
             {"Gamma", EngineConf.renderer.gamma, ApplyPolicy::RestartNotNeeded},
             {"Lod0 distance ", EngineConf.renderer.lodDistance0, ApplyPolicy::RestartNotNeeded},
             {"Lod1 distance", EngineConf.renderer.lodDistance1, ApplyPolicy::RestartNotNeeded},
             //{"Lod2 distance", EngineConf.renderer.lodDistance2, ApplyPolicy::RestartNotNeeded},
             {"Textures size limit", EngineConf.renderer.textures.limitTextureSize, ApplyPolicy::RestartRequired},
             {"Textures max size", EngineConf.renderer.textures.maxSize, ApplyPolicy::RestartRequired},
             //{"Use ambient texture", EngineConf.renderer.textures.useAmbient, true},
             {"Use normal texture", EngineConf.renderer.textures.useNormal, ApplyPolicy::RestartRequired},
             {"Use diffuse texture", EngineConf.renderer.textures.useDiffuse, ApplyPolicy::RestartNotNeeded},
             //{"Use specular texture", EngineConf.renderer.textures.useSpecular, true},
             //{"Use displacement texture", EngineConf.renderer.textures.useDisplacement, true},
             {"Flora", EngineConf.renderer.flora.isEnabled, ApplyPolicy::RestartRequired},
             {"Flora view distance", EngineConf.renderer.flora.viewDistance, ApplyPolicy::RestartNotNeeded},
             {"Normal mapping distance", EngineConf.renderer.normalMappingDistance, ApplyPolicy::RestartRequired},
             {"Use particles", EngineConf.renderer.particles.useParticles, ApplyPolicy::RestartRequired},
             {"Water type", EngineConf.renderer.water.type, ApplyPolicy::RestartNotNeeded},
             {"Water reflection texture size", EngineConf.renderer.water.waterReflectionResolution, ApplyPolicy::RestartNotNeeded},
             {"Water refraction texture size", EngineConf.renderer.water.waterRefractionResolution, ApplyPolicy::RestartNotNeeded},
             {"Shadows", EngineConf.renderer.shadows.isEnabled, ApplyPolicy::RestartNotNeeded},
             {"Shadow first casade distance", EngineConf.renderer.shadows.firstCascadeDistance, ApplyPolicy::RestartRequired},
             {"Shadows distance", EngineConf.renderer.shadows.distance, ApplyPolicy::RestartRequired},
             {"Shadow map size", EngineConf.renderer.shadows.mapSize, ApplyPolicy::RestartRequired},
             {"Shadows cascades size", EngineConf.renderer.shadows.cascadesSize, ApplyPolicy::RestartRequired},
             {"Terrain mesh resolution divide factor", EngineConf.renderer.terrain.resolutionDivideFactor, ApplyPolicy::RestartNotNeeded},
             {"Terrain mesh parts count", EngineConf.renderer.terrain.meshPartsCount, ApplyPolicy::RestartNotNeeded}
          }
        });
    categories.push_back({"Sound",
                          {
                              {"Enabled", EngineConf.sound.isEnabled, ApplyPolicy::RestartNotNeeded},
                              {"Volume", EngineConf.sound.volume, ApplyPolicy::RestartNotNeeded}
                          }
                        });
    categories.push_back(
        {"Debug",
          {
             {"Logger lvl", EngineConf.debugParams.logLvl, ApplyPolicy::RestartNotNeeded},
             {"Show render info", EngineConf.debugParams.showRenderInfo, ApplyPolicy::RestartNotNeeded},
             {"Show renders timers", EngineConf.debugParams.showRenderersTimers, ApplyPolicy::RestartRequired},
             {"Enable physics visualization", EngineConf.debugParams.showPhycicsVisualization, ApplyPolicy::RestartNotNeeded}
          }
        });

    // clang-format on
}
const ConfigurationExplorer::Categories& ConfigurationExplorer::getParamsByCategories() const
{
    return categories;
}
const ConfigurationExplorer::Params& ConfigurationExplorer::getParamsFromCategoryConst(const std::string& name) const
{
    auto iter = std::find_if(categories.begin(), categories.end(),
                             [&name](const auto& category) { return category.name == name; });

    if (iter != categories.end())
        return iter->params;

    return emptyParams;
}

ConfigurationExplorer::Params &ConfigurationExplorer::getParamsFromCategory(const std::string & name)
{
    auto iter = std::find_if(categories.begin(), categories.end(),
                             [&name](const auto& category) { return category.name == name; });

    if (iter != categories.end())
        return iter->params;

    return emptyParams;
}
std::vector<std::string> ConfigurationExplorer::getCatogiresList() const
{
    std::vector<std::string> result;
    for (const auto& category : categories)
    {
        result.push_back(category.name);
    }
    return result;
}
}  // namespace GameEngine
