#pragma once
#include <Types.h>

#include <memory>
#include <optional>

#include "GameEngine/Camera/CameraWrapper.h"
#include "GameEngine/Renderers/Projection.h"

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

struct TerrainPoint
{
    vec2 pointOnTerrain;
    HeightMap& heightMap;
};

class TerrainPainter
{
    using Terrain = Components::TerrainRendererComponent;

public:
    TerrainPainter(const CameraWrapper&, const Projection&, const vec2ui&, const Components::ComponentController&);
    void PaintBlendMap(const vec2&, const vec3& color, float range);
    void PaintHeightMap(const vec2&, float height, float range);

private:
    void GetAllSceneTerrains();
    std::optional<TerrainPoint> GetMousePointOnTerrain(const vec2& mousePosition);
    bool IntersectionInRange(float start, float finish, const vec3& ray);
    vec3 GetPointOnRay(const vec3& ray, float distance);
    bool IsUnderGround(const vec3& testPoint);
    Terrain* GetTerrain(float worldX, float worldZ);
    std::optional<TerrainPoint> BinarySearch(int count, float start, float finish, const vec3& ray);

private:
    const CameraWrapper& camera_;
    const Projection& projection_;
    const vec2ui& windowSize_;
    const Components::ComponentController& componentController_;

    std::vector<Components::TerrainRendererComponent*> terrains_;
};
}  // namespace GameEngine
