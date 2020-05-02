#pragma once
#include <Types.h>

#include <memory>
#include <optional>

#include "GameEngine/Camera/CameraWrapper.h"
#include "GameEngine/Renderers/Projection.h"
#include "TerrainPoint.h"

namespace common
{
class Transform;
}  // namespace common

namespace GameEngine
{
class GameObject;
class HeightMap;

namespace Components
{
class ComponentController;
class TerrainRendererComponent;
}  // namespace Components

class TerrainPointGetter
{
    using Terrain = Components::TerrainRendererComponent;

public:
    TerrainPointGetter(const CameraWrapper&, const Projection&, const vec2ui&, const Components::ComponentController&);
    std::optional<TerrainPoint> GetMousePointOnTerrain(const vec2& mousePosition);

private:
    void GetAllSceneTerrains();
    bool IntersectionInRange(float start, float finish, const vec3& ray);
    vec3 GetPointOnRay(const vec3& ray, float distance);
    bool IsUnderGround(const vec3& testPoint);
    Terrain* GetTerrain(float worldX, float worldZ);
    std::optional<TerrainPoint> BinarySearch(uint32 count, float start, float finish, const vec3& ray);
    vec2ui CastToTerrainSpace(Terrain&, const vec3&);

private:
    const CameraWrapper& camera_;
    const Projection& projection_;
    const vec2ui& windowSize_;
    const Components::ComponentController& componentController_;

    std::vector<Components::TerrainRendererComponent*> terrains_;
};
}  // namespace GameEngine
