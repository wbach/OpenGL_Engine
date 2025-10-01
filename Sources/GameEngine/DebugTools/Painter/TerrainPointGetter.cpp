#include "TerrainPointGetter.h"

#include <Common/Transform.h>
#include <Logger/Log.h>

#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"
#include "GameEngine/Components/Physics/Terrain/TerrainShape.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/DebugTools/Common/MouseUtils.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Camera/CameraWrapper.h"
#include "TerrainPoint.h"

namespace GameEngine
{
namespace
{
const uint32 RECURSION_COUNT{200};
const uint32 RAY_RANGE{100};
}  // namespace

TerrainPointGetter::TerrainPointGetter(const CameraWrapper& camera, const Projection& projection,
                                       const vec2ui& windowSize,
                                       const Components::ComponentController& componentController)
    : camera_(camera)
    , projection_(projection)
    , componentController_(componentController)
{
}

std::vector<Components::TerrainRendererComponent*> TerrainPointGetter::GetSceneTerrains() const
{
    return componentController_.GetAllComponentsOfType<Components::TerrainRendererComponent>();
}

std::optional<TerrainPoint> TerrainPointGetter::GetMousePointOnTerrain(const vec2& mousePosition)
{
    terrains_ = GetSceneTerrains();

    if (not terrains_.empty())
    {
        auto ray = CalculateMouseRayDirection(projection_, camera_, mousePosition);

        if (IntersectionInRange(0, RAY_RANGE, ray))
        {
            return BinarySearch(0, 0, RAY_RANGE, ray);
        }
    }

    return std::nullopt;
}

bool TerrainPointGetter::IntersectionInRange(float start, float finish, const vec3& ray)
{
    auto startPoint = GetPointOnRay(ray, start);
    auto endPoint   = GetPointOnRay(ray, finish);
    return (not IsUnderGround(startPoint) and IsUnderGround(endPoint));
}

vec3 TerrainPointGetter::GetPointOnRay(const vec3& ray, float distance)
{
    vec3 scaledRay(ray.x * distance, ray.y * distance, ray.z * distance);
    return (camera_.GetPosition() + scaledRay);
}

bool TerrainPointGetter::IsUnderGround(const vec3& testPoint)
{
    const auto& terrain = GetTerrain(testPoint.x, testPoint.z);

    if (not terrain or not terrain->GetHeightMap())
    {
        /* LOG TO FIX*/  LOG_ERROR << ("No terrain or height map in terrain.");
        return false;
    }
    TerrainHeightGetter terrainHeightGetter(terrain->getParentGameObject().GetWorldTransform().GetScale(),
                                            *terrain->GetHeightMap(),
                                            terrain->GetParentGameObject().GetLocalTransform().GetPosition());

    auto height = terrainHeightGetter.GetHeightofTerrain(testPoint.x, testPoint.z);

    if (height)
    {
        return (testPoint.y <= *height);
    }
    return false;
}

TerrainPointGetter::Terrain* TerrainPointGetter::GetTerrain(float worldX, float worldZ)
{
    // int x = static_cast<int>(worldX / TERRAIN_SIZE);
    // int z = static_cast<int>(worldZ / TERRAIN_SIZE);

    // int i = x + m_TerrainsHeight * z;

    // if (i > terrains_.size() or i < 0)
    //    return nullptr;

    return terrains_[0];
}

std::optional<TerrainPoint> TerrainPointGetter::BinarySearch(uint32 count, float start, float finish, const vec3& ray)
{
    float half = start + ((finish - start) / 2.0f);
    if (count >= RECURSION_COUNT)
    {
        auto worldPointOnTerrain = GetPointOnRay(ray, half);

        auto terrain = GetTerrain(worldPointOnTerrain.x, worldPointOnTerrain.z);
        if (not terrain)
            return std::nullopt;

        auto terrainSpacePoint = CastToTerrainSpace(*terrain, worldPointOnTerrain);
        TerrainPoint result{worldPointOnTerrain, terrainSpacePoint, *terrain};
        return result;
    }
    if (IntersectionInRange(start, half, ray))
    {
        return BinarySearch(count + 1, start, half, ray);
    }
    else
    {
        return BinarySearch(count + 1, half, finish, ray);
    }
}
vec2 TerrainPointGetter::CastToTerrainSpace(Terrain& terrain, const vec3& worldPointOnTerrain)
{
    const auto& scale = terrain.GetParentGameObject().GetWorldTransform().GetScale();
    vec2 result;
    auto halfScale       = scale / 2.f;
    auto terrainPosition = terrain.GetParentGameObject().GetWorldTransform().GetPosition();
    result.x             = (worldPointOnTerrain.x + halfScale.x + terrainPosition.x) / scale.x;
    result.y             = (worldPointOnTerrain.z + halfScale.z + terrainPosition.z) / scale.z;
    // point on terrain in <0, 1>
    return result;
}
}  // namespace GameEngine
