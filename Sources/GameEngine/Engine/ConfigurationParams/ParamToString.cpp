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
}  // namespace Params
}  // namespace GameEngine
