#pragma once
#include <Types.h>

#include <functional>
#include <string>
#include <vector>

namespace GameEngine
{
namespace Params
{
class IConfigurationParam
{
public:
    virtual ~IConfigurationParam() = default;

    virtual std::string toString() const                        = 0;
    virtual std::string next()                                  = 0;
    virtual std::string previous()                              = 0;
    virtual void apply()                                        = 0;
    virtual IdType subscribeForChange(std::function<void()>)    = 0;
    virtual void unsubscribe(const IdType&)                     = 0;
    virtual void setValueFromIndex(size_t)                      = 0;
    virtual size_t getValueIndex() const                        = 0;
    virtual std::vector<std::string> getValuesAsStrings() const = 0;
    virtual void lock()                                         = 0;
    virtual void unlock()                                       = 0;
    virtual bool isLocked() const                               = 0;
};
}  // namespace Params
}  // namespace GameEngine
