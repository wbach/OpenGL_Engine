#pragma once
#include "Types.h"

namespace GameEngine
{
namespace Collada
{
struct Matrix
{
    mat4 matrix_{1.f};
    std::string sid_{""};
};
}
}
