#pragma once
#include <vector>
#include <string>

namespace GameEngine
{
namespace Params
{
class IConfigurationParam;
}

class ConfigurationExplorer
{
public:
    enum class ApplyPolicy
    {
        RestartRequired,
        RestartNotNeeded
    };
    enum class ParamsImpact
    {
        HasImpact,
        NoImpact
    };
    struct Param
    {
        std::string name;
        GameEngine::Params::IConfigurationParam& configurationParam;
        ApplyPolicy restartRequierd;
        ParamsImpact paramsImpact{ParamsImpact::NoImpact};
    };
    using Params = std::vector<Param>;
    struct Category
    {
        std::string name;
        Params params;
    };

    using Categories = std::vector<Category>;

    ConfigurationExplorer();
    const Categories& getParamsByCategories() const;
    const Params& getParamsFromCategoryConst(const std::string&) const;
    Params& getParamsFromCategory(const std::string&);
    std::vector<std::string> getCatogiresList() const;

private:
    Categories categories;
};
}  // namespace GameEngine
