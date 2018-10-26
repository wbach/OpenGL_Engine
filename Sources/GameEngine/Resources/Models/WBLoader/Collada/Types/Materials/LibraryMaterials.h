#pragma once
#include <unordered_map>
#include "Material.h"
#include "Types.h"

namespace GameEngine
{
namespace Collada
{
struct LibraryMaterials
{
    std::unordered_map<std::string, Material> materials_;
};
}
}
