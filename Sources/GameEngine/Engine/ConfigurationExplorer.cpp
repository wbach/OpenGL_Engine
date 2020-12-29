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
    categories.push_back({"Graphics",
                          {
                              {"Window Size", EngineConf.window.size},
                              {"Fullscreen mode", EngineConf.window.fullScreen},
                              {"Water Type", EngineConf.renderer.water.type},
                              {"Water Reflection texture size", EngineConf.renderer.water.waterReflectionResolution},
                              {"Water Refraction texture size", EngineConf.renderer.water.waterRefractionResolution},
                              {"Shadows", EngineConf.renderer.shadows.isEnabled},
                          }});
    categories.push_back({"Sound",
                          {
                              {"Enabled", EngineConf.sound.isEnabled},
                              {"Volume", EngineConf.sound.volume},
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
