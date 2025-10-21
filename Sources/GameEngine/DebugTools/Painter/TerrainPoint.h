#pragma once
#include <Types.h>

#include <ostream>

namespace GameEngine
{
namespace Components
{
class TerrainRendererComponent;
}  // namespace Components

struct TerrainPoint
{
    vec3 pointOnTerrain;
    vec2 terrainSpacePoint;
    Components::TerrainRendererComponent* terrainComponent{nullptr};
};

inline std::ostream& operator<<(std::ostream& os, const TerrainPoint& tp)
{
    os << "TerrainPoint { "
       << "pointOnTerrain = (" << tp.pointOnTerrain.x << ", " << tp.pointOnTerrain.y << ", " << tp.pointOnTerrain.z << "), "
       << "terrainSpacePoint = (" << tp.terrainSpacePoint.x << ", " << tp.terrainSpacePoint.y << "), "
       << "terrainComponent = " << (tp.terrainComponent ? "valid" : "nullptr") << " }";
    return os;
}
}  // namespace GameEngine
