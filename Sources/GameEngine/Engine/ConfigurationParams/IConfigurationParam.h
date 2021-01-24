#pragma once
#include <string>

namespace GameEngine
{
namespace Params
{
class IConfigurationParam
{
public:
    virtual ~IConfigurationParam()       = default;
    virtual std::string toString() const = 0;
    virtual std::string next()           = 0;
    virtual std::string previous()       = 0;
    virtual void apply()                 = 0;
};
}  // namespace Params
}  // namespace GameEngine
