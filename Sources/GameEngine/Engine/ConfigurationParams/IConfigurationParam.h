#pragma once
#include <string>

namespace GameEngine
{
namespace Params
{
class IConfigurationParam
{
public:
    enum class Type
    {
        Boolean,
        Float,
        Integer,
        String,
        Vec2,
        Vec2i,
        Vec3,
        Vec3i,
        Enum
    };
    virtual ~IConfigurationParam()       = default;
    virtual std::string toString() const = 0;
    virtual std::string next()           = 0;
    virtual std::string previous()       = 0;
    virtual void apply()                 = 0;
    virtual Type getType() const         = 0;
};
}  // namespace Params
}  // namespace GameEngine
