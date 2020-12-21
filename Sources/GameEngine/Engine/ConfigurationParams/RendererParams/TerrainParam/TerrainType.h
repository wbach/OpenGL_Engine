#pragma once
#include <optional>
#include <string>

namespace GameEngine
{
namespace Params
{
enum class TerrainType
{
    Mesh,
    Tessellation
};
}
}  // namespace GameEngine

namespace std
{
void from_string(const std::string&, GameEngine::Params::TerrainType&);
std::string to_string(GameEngine::Params::TerrainType);
}  // namespace std
