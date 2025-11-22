#pragma once
#include <Types.h>

#include <optional>

namespace common
{
class Transform;
}  // namespace common

namespace GameEngine
{
class GameObject;
class HeightMap;
class ICamera;
struct TerrainPoint;

namespace Components
{
class ComponentController;
class TerrainRendererComponent;
}  // namespace Components

class TerrainPointGetter
{
    using Terrain = Components::TerrainRendererComponent;

public:
    TerrainPointGetter(const ICamera&, const Components::ComponentController&);
    std::optional<TerrainPoint> GetMousePointOnTerrain(const vec2& mousePosition);
    std::vector<Components::TerrainRendererComponent*> GetSceneTerrains() const;

private:
    bool IntersectionInRange(double start, double finish, const vec3d& ray);
    vec3d GetPointOnRay(const vec3d& ray, double distance);
    bool IsUnderGround(const vec3d& testPoint);
    Terrain* GetTerrain(double worldX, double worldZ);
    std::optional<TerrainPoint> BinarySearch(uint32 count, double start, double finish, const vec3d& ray);
    vec2d CastToTerrainSpace(Terrain&, const vec3d&);

private:
    const ICamera& camera_;
    const Components::ComponentController& componentController_;

    std::vector<Components::TerrainRendererComponent*> terrains_;
};
}  // namespace GameEngine
