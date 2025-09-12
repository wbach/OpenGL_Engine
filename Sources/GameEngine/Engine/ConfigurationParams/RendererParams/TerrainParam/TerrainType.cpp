#include "TerrainType.h"
#include <Logger/Log.h>

#define Return(x)                                   \
    if (type == GameEngine::Params::TerrainType::x) \
    return (#x)

namespace std
{
void from_string(const std::string& type, GameEngine::Params::TerrainType& output)
{
    // clang-format off
    if (type == "Mesh") { output = GameEngine::Params::TerrainType::Mesh; return; }
    if (type == "Tessellation") { output = GameEngine::Params::TerrainType::Tessellation; return; }
    // clang-format on
    /* LOG TO FIX*/  LOG_ERROR << ("can not convert str to enum : " + type);
}
std::string to_string(GameEngine::Params::TerrainType type)
{
    Return(Mesh);
    Return(Tessellation);
    return "unknown";
}
}  // namespace std
