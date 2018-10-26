#pragma once
#include "Array.h"
#include "TechniqueCommon.h"
#include "Types.h"

namespace GameEngine
{
namespace Collada
{
struct Source
{
    std::string id;
    TechniqueCommon technique;
    Array dataArray;
};
}
}
