#pragma once
#include <Types.h>

#include <functional>
#include <string>
namespace GameEngine
{
namespace Params
{
class IConfigurationParam
{
public:
    virtual ~IConfigurationParam() = default;

    virtual std::string toString() const                     = 0;
    virtual std::string next()                               = 0;
    virtual std::string previous()                           = 0;
    virtual void apply()                                     = 0;
    virtual IdType subscribeForChange(std::function<void()>) = 0;
    virtual void unsubscribe(const IdType&)                  = 0;
};
}  // namespace Params
}  // namespace GameEngine
