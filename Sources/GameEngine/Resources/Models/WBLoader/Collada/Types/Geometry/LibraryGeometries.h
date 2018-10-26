#pragma once
#include <unordered_map>
#include "Geometry.h"
#include "Types.h"

namespace GameEngine
{
namespace Collada
{
struct LibraryGeometries
{
    std::unordered_map<std::string, Geometry> geometries_;
};
}
}
