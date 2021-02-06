#include "ConfigurationExplorer.h"

#include <algorithm>

#include "Configuration.h"

namespace GameEngine
{
namespace
{
const ConfigurationExplorer::Params emptyParams;
}
ConfigurationExplorer::ConfigurationExplorer()
{
    categories.push_back(
        {"Graphics",
         {
             {"Api", EngineConf.renderer.graphicsApi, true},
             {"Renderer type", EngineConf.renderer.type, true},
             {"Fullscreen mode", EngineConf.window.fullScreen, false},
             {"Window size", EngineConf.window.size, false},
             {"Render size", EngineConf.renderer.resolution, true},
             {"View distance", EngineConf.renderer.viewDistance, false},
             {"Textures max size", EngineConf.renderer.textures.maxSize, true},
             //{"Use ambient texture", EngineConf.renderer.textures.useAmbient, true},
             {"Use normal texture", EngineConf.renderer.textures.useNormal, true},
             //{"Use specular texture", EngineConf.renderer.textures.useSpecular, true},
             //{"Use displacement texture", EngineConf.renderer.textures.useDisplacement, true},
             {"Fps limit", EngineConf.renderer.fpsLimt, true},
             {"Flora", EngineConf.renderer.flora.isEnabled, true},
             {"Flora view distance", EngineConf.renderer.flora.viewDistance, true},
             {"Normal mapping distance", EngineConf.renderer.normalMappingDistance, true},
             {"Use particles", EngineConf.renderer.particles.useParticles, true},
             {"Water type", EngineConf.renderer.water.type, false},
             {"Water reflection texture size", EngineConf.renderer.water.waterReflectionResolution, false},
             {"Water refraction texture size", EngineConf.renderer.water.waterRefractionResolution, false},
             {"Shadows", EngineConf.renderer.shadows.isEnabled, false},
             {"Shadows distance", EngineConf.renderer.shadows.distance, true},
             {"Shadow map size", EngineConf.renderer.shadows.mapSize, true},
             {"Shadows cascades size", EngineConf.renderer.shadows.cascadesSize, true},
             {"Terrain mesh resolution divide factor", EngineConf.renderer.terrain.resolutionDivideFactor, false},
             {"Terrain mesh parts count", EngineConf.renderer.terrain.meshPartsCount, false},
         }});
    categories.push_back({"Sound",
                          {
                              {"Enabled", EngineConf.sound.isEnabled, false},
                              {"Volume", EngineConf.sound.volume, false},
                          }});
    categories.push_back({"Debug",
                          {
                              {"Logger lvl", EngineConf.debugParams.logLvl, false},
                              {"Show render info", EngineConf.debugParams.showRenderInfo, false}
                          }});
}
const ConfigurationExplorer::Categories& ConfigurationExplorer::getParamsByCategories() const
{
    return categories;
}
const ConfigurationExplorer::Params& ConfigurationExplorer::getParamsFromCategory(const std::string& name) const
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
