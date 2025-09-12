#include "ParamToString.h"

#include <Utils/Utils.h>

#include <sstream>

namespace GameEngine
{
namespace Params
{
std::string paramToString(int i)
{
    return std::to_string(i);
}

std::string paramToString(uint32 i)
{
    return std::to_string(i);
}

std::string paramToString(bool v)
{
    return Utils::BoolToString(v);
}

std::string paramToString(float f)
{
    std::ostringstream out;
    out.precision(2);
    out << std::fixed << f;
    return out.str();
}

std::string paramToString(const vec2ui& v)
{
    return std::to_string(v.x) + "x" + std::to_string(v.y);
}
std::string paramToString(const std::string& v)
{
    return v;
}
std::string paramToString(GraphicsApi::RendererType renderType)
{
    if (renderType == GraphicsApi::RendererType::FULL)
    {
        return "full";
    }

    return "simple";
}

std::string paramToString(LoggingLvl lvl)
{
    switch (lvl)
    {
        case LoggingLvl::None:
            return "none";
        case LoggingLvl::Error:
            return "error";
        case LoggingLvl::ErrorWarning:
            return "error/warning";
        case LoggingLvl::ErrorWarningInfo:
            return "error/warning/info";
        case LoggingLvl::ErrorWarningInfoDebug:
            return "error/warning/info/debug";
    }
    return {"unknown"};
}

LoggingLvl paramFromString(const std::string & input)
{
    auto str = Utils::toLower(input);
    if (str=="none")
        return LoggingLvl::None;
    if (str == "error" or str == "error")
        return LoggingLvl::Error;
    if (str == "error/warning" or str == "warning")
        return LoggingLvl::ErrorWarning;
    if (str == "error/warning/info" or str == "info")
        return LoggingLvl::ErrorWarningInfo;
    if (str == "error/warning/info/debug" or str == "debug")
        return LoggingLvl::ErrorWarningInfoDebug;

    return LoggingLvl::None;
}

}  // namespace Params
}  // namespace GameEngine
