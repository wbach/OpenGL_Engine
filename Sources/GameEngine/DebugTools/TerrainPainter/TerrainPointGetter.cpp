#include "TerrainPointGetter.h"

#include <Common/Transform.h>

#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"
#include "GameEngine/Components/Physics/Terrain/TerrainShape.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"

#include "GameEngine/DebugTools/Common/MouseUtils.h"
#include "GameEngine/Objects/GameObject.h"

#include <Logger/Log.h>

#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
namespace
{
const uint32 RECURSION_COUNT{200};
const uint32 RAY_RANGE{600};
}  // namespace

TerrainPointGetter::TerrainPointGetter(const CameraWrapper& camera, const Projection& projection,
                                       const vec2ui& windowSize,
                                       const Components::ComponentController& componentController)
    : camera_(camera)
    , projection_(projection)
    , windowSize_(windowSize)
    , componentController_(componentController)
{
}

void TerrainPointGetter::GetAllSceneTerrains()
{
    terrains_.clear();

    const auto& components = componentController_.GetAllComonentsOfType(Components::ComponentsType::TerrainRenderer);

    for (auto& terrain : components)
    {
        terrains_.push_back(static_cast<Components::TerrainRendererComponent*>(terrain.second));
    }
}

std::optional<TerrainPoint> TerrainPointGetter::GetMousePointOnTerrain(const vec2& mousePosition)
{
    GetAllSceneTerrains();

    if (not terrains_.empty())
    {
        auto ray = CalculateMouseRay(projection_, camera_, mousePosition);

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

    if (not terrain and not terrain->GetHeightMap())
    {
        ERROR_LOG("No terrain or height map in terrain.");
        return false;
    }

    TerrainHeightGetter terrainHeightGetter(terrain->GetTerrainConfiguration(), *terrain->GetHeightMap(),
                                            terrain->GetParentGameObject().GetTransform().GetPosition());

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
        auto pointOnRay = GetPointOnRay(ray, half);

        auto terrain = GetTerrain(pointOnRay.x, pointOnRay.z);
        if (not terrain)
            return std::nullopt;

        DEBUG_LOG("Result : " + std::to_string(pointOnRay));

        vec2ui pointOnTerrain(static_cast<uint32>(pointOnRay.x), static_cast<uint32>(pointOnRay.z));
        TerrainPoint result{pointOnTerrain, *terrain};
        return result;
    }
    if (IntersectionInRange(start, half, ray))
    {
        DEBUG_LOG("first part inter");
        return BinarySearch(count + 1, start, half, ray);
    }
    else
    {
        DEBUG_LOG("sec part inter");
        return BinarySearch(count + 1, half, finish, ray);
    }
}
}  // namespace GameEngine
