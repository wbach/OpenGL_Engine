#pragma once
#include <vector>
#include "ConfigurationParams/IConfigurationParam.h"

namespace GameEngine
{
class ConfigurationExplorer
{
public:
    struct Param
    {
        std::string name;
        Params::IConfigurationParam& configurationParam;
        bool restartRequierd;
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
    const Params& getParamsFromCategory(const std::string&) const;
    std::vector<std::string> getCatogiresList() const;

private:
    Categories categories;
};
}  // namespace GameEngine
