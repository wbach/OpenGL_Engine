#pragma once
#include <list>
#include "Param.h"
#include "Types.h"

namespace GameEngine
{
namespace Collada
{
struct Accessor
{
    std::string source;
    uint32 stride;
    uint32 count;
    std::list<Param> params;
};
}
}
