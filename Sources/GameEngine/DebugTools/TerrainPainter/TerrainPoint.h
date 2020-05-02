#pragma once
#include <Types.h>

namespace GameEngine
{
namespace Components
{
class TerrainRendererComponent;
}  // namespace Components

struct TerrainPoint
{
    vec3 pointOnTerrain;
    vec2ui pointOnHeightMap;
    Components::TerrainRendererComponent& terrainComponent;
};
}  // namespace GameEngine
