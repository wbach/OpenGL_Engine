#pragma once
#include "Types.h"

namespace GameEngine
{
namespace Collada
{
struct Color
{
    std::string sid;
    vec4 value;
    std::string texture;
    std::string texcoord;
};
}
}
