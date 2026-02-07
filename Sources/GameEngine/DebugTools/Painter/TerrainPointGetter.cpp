#include "TerrainPointGetter.h"

#include <Common/Transform.h>
#include <Logger/Log.h>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/DebugTools/Common/MouseUtils.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "TerrainPoint.h"

namespace GameEngine
{
namespace
{
constexpr uint32 RECURSION_COUNT{50};
constexpr double RAY_RANGE{1000.0};
using vec3d = glm::dvec3;
}  // namespace

TerrainPointGetter::TerrainPointGetter(const ICamera& camera, const Components::ComponentController& componentController)
    : camera_(camera)
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
        auto ray = CalculateMouseRay(camera_, mousePosition);
        vec3d rayDirD{ray.direction};

        if (IntersectionInRange(0.0, RAY_RANGE, rayDirD))
        {
            return BinarySearch(0, 0.0, RAY_RANGE, rayDirD);
        }
    }

    return std::nullopt;
}

bool TerrainPointGetter::IntersectionInRange(double start, double finish, const vec3d& ray)
{
    auto startPoint = GetPointOnRay(ray, start);
    auto endPoint   = GetPointOnRay(ray, finish);

    bool startUnder = IsUnderGround(startPoint);
    bool endUnder   = IsUnderGround(endPoint);

    return (startUnder != endUnder);
}

vec3d TerrainPointGetter::GetPointOnRay(const vec3d& ray, double distance)
{
    vec3 camPos{camera_.GetPosition()};
    return vec3d(camPos) + ray * distance;
}

bool TerrainPointGetter::IsUnderGround(const vec3d& testPoint)
{
    const auto& terrain = GetTerrain(testPoint.x, testPoint.z);
    if (not terrain)
        return false;

    auto maybeHeight =
        TerrainHeightGetter(terrain->getParentGameObject().GetWorldTransform().GetScale(), *terrain->GetHeightMap(),
                            terrain->GetParentGameObject().GetLocalTransform().GetPosition())
            .GetHeightofTerrain(testPoint.x, testPoint.z);

    if (not maybeHeight)
    {
        // punkt poza terenem → interpretujemy jako "poniżej terenu" jeśli patrzymy w dół,
        // lub "powyżej" jeśli patrzymy w górę
        // dla promienia w dół:
        return testPoint.y <= terrain->getParentGameObject().GetWorldTransform().GetPosition().y;
    }

    return testPoint.y <= *maybeHeight;
}

TerrainPointGetter::Terrain* TerrainPointGetter::GetTerrain(double worldX, double worldZ)
{
    if (terrains_.empty())
        return nullptr;

    // TODO: wybór odpowiedniego terenu na podstawie worldX, worldZ
    return terrains_[0];
}

std::optional<TerrainPoint> TerrainPointGetter::BinarySearch(uint32 count, double start, double finish, const vec3d& ray)
{
    double half = start + ((finish - start) / 2.0);

    if (count >= RECURSION_COUNT)
    {
        auto worldPointOnTerrain = GetPointOnRay(ray, half);
        auto terrain             = GetTerrain(worldPointOnTerrain.x, worldPointOnTerrain.z);

        if (not terrain)
        {
            return std::nullopt;
        }

        TerrainHeightGetter terrainHeightGetter(terrain->getParentGameObject().GetWorldTransform().GetScale(),
                                                *terrain->GetHeightMap(),
                                                terrain->getParentGameObject().GetLocalTransform().GetPosition());

        auto terrainHeight = terrainHeightGetter.GetHeightofTerrain(vec2d(worldPointOnTerrain.x, worldPointOnTerrain.z));

        if (terrainHeight)
            worldPointOnTerrain.y = *terrainHeight;

        auto terrainSpacePoint = CastToTerrainSpace(*terrain, worldPointOnTerrain);

        if (terrainSpacePoint.x < 0.0 || terrainSpacePoint.x > 1.0 || terrainSpacePoint.y < 0.0 || terrainSpacePoint.y > 1.0)
        {
            return std::nullopt;
        }

        TerrainPoint result{
            .pointOnTerrain    = vec3(static_cast<float>(worldPointOnTerrain.x), static_cast<float>(worldPointOnTerrain.y),
                                      static_cast<float>(worldPointOnTerrain.z)),
            .terrainSpacePoint = vec2(static_cast<float>(terrainSpacePoint.x), static_cast<float>(terrainSpacePoint.y)),
            .terrainComponent  = terrain};

        return result;
    }

    if (IntersectionInRange(start, half, ray))
        return BinarySearch(count + 1, start, half, ray);
    else
        return BinarySearch(count + 1, half, finish, ray);
}

vec2d TerrainPointGetter::CastToTerrainSpace(Terrain& terrain, const vec3d& worldPointOnTerrain)
{
    const auto scale = terrain.GetParentGameObject().GetWorldTransform().GetScale() * terrain.GetHeightMap()->GetScale();
    auto terrainPos  = terrain.GetParentGameObject().GetWorldTransform().GetPosition();

    vec2 result;
    result.x = (worldPointOnTerrain.x - terrainPos.x) / scale.x + 0.5;
    result.y = (worldPointOnTerrain.z - terrainPos.z) / scale.z + 0.5;

    return result;
}

}  // namespace GameEngine
