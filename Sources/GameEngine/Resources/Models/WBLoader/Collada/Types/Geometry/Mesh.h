#pragma once
#include <unordered_map>
#include "GameEngine/Resources/Models/WBLoader/Collada/Types/Common/Source.h"
#include "Polylist.h"
#include "Types.h"
#include "Vertices.h"

namespace GameEngine
{
namespace Collada
{
struct Mesh
{
    std::unordered_map<std::string, Source> sources_;
    std::unordered_map<std::string, Vertices> vertices_;
    std::unordered_map<std::string, Polylist> polylist_;
};
}
}
