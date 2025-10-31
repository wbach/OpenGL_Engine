#include "TerrainHeightGetter.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GLM/GLMUtils.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
TerrainHeightGetter::TerrainHeightGetter(Components::TerrainRendererComponent& component)
    : TerrainHeightGetter(
          component.GetParentGameObject().GetWorldTransform().GetScale(),
          [&]() -> HeightMap&
          {
              auto* texture   = component.GetTexture(TerrainTextureType::heightmap);
              auto* heightMap = dynamic_cast<HeightMap*>(texture);

              if (!heightMap)
                  throw std::runtime_error("TerrainHeightGetter: heightmap texture is null or invalid type");

              return *heightMap;
          }(),
          component.GetParentGameObject().GetWorldTransform().GetPosition())
{
}

TerrainHeightGetter::TerrainHeightGetter(const vec3& terrainScale, const HeightMap& heightMap, const vec3& terrainPosition)
    : terrainScale_(terrainScale)
    , heightMap_(heightMap)
    , terrainPosition_(terrainPosition)
    , tools_(terrainScale, heightMap.GetImage())
{
    heightMapResolution_ = heightMap_.GetImage().width;
    gridSquereSize_      = terrainScale / static_cast<float>(heightMapResolution_ - 1);
}

std::optional<float> TerrainHeightGetter::GetHeightofTerrain(float worldX, float worldZ) const
{
    return GetHeightofTerrain(vec2d(worldX, worldZ));
}

std::optional<vec3> TerrainHeightGetter::GetPointOnTerrain(float worldX, float worldZ) const
{
    auto height = GetHeightofTerrain(worldX, worldZ);
    if (height)
        return vec3(worldX, *height, worldZ);

    return std::nullopt;
}

std::optional<vec3> TerrainHeightGetter::GetNormalOfTerrain(float worldX, float worldZ) const
{
    auto localPosition = GetLocalPositionOnTerrain(vec2d(worldX, worldZ));

    if (not localPosition)
        return std::nullopt;

    auto gridCoord = GetGridCoord(*localPosition);
    if (not gridCoord)
        return std::nullopt;

    return GetNormalInTerrainQuad(*gridCoord, *localPosition);
}

std::optional<float> TerrainHeightGetter::GetHeightofTerrain(const vec2& worldPosition) const
{
    auto localPosition = GetLocalPositionOnTerrain(worldPosition);

    if (not localPosition)
        return std::nullopt;

    auto gridCoord = GetGridCoord(*localPosition);
    if (not gridCoord)
        return std::nullopt;

    return GetHeightInTerrainQuad(*gridCoord, *localPosition);
}

std::optional<double> TerrainHeightGetter::GetHeightofTerrain(const vec2d& worldPosition) const
{
    auto localPosition = GetLocalPositionOnTerrain(worldPosition);

    if (not localPosition)
        return std::nullopt;

    auto gridCoord = GetGridCoord(*localPosition);
    if (not gridCoord)
        return std::nullopt;

    return GetHeightInTerrainQuad(*gridCoord, *localPosition);
}

std::optional<vec2d> TerrainHeightGetter::GetLocalPositionOnTerrain(const vec2d& worldPostion) const
{
    vec3d halfTerrainScale = terrainScale_ / 2.0f;
    auto terrain_x         = worldPostion.x + halfTerrainScale.x - terrainPosition_.x;
    auto terrain_z         = worldPostion.y + halfTerrainScale.z - terrainPosition_.z;

    if (terrain_x < 0.0 or terrain_x > terrainScale_.x or terrain_z < 0.0 or terrain_z > terrainScale_.z)
        return std::nullopt;

    return vec2d(terrain_x, terrain_z);
}

std::optional<vec2ui> TerrainHeightGetter::GetGridCoord(const vec2d& position) const
{
    auto xi = static_cast<int32>(floor(position.x / gridSquereSize_.x));
    auto yi = static_cast<int32>(floor(position.y / gridSquereSize_.z));

    if (not IsValidGridCoordinate(vec2i(xi, yi)))
        return std::nullopt;

    return vec2ui(static_cast<uint32>(xi), static_cast<uint32>(yi));
}

vec2d TerrainHeightGetter::GetPositionInQuad(const vec2d& position) const
{
    double x_coord = fmod(position.x, gridSquereSize_.x) / gridSquereSize_.x;
    double z_coord = fmod(position.y, gridSquereSize_.z) / gridSquereSize_.z;

    return vec2d(x_coord, z_coord);
}

bool TerrainHeightGetter::IsInLeftTriangle(const vec2d& position) const
{
    return position.x <= (1.0 - position.y);
}

bool TerrainHeightGetter::IsValidGridCoordinate(const vec2i& position) const
{
    auto heightMapResolution = static_cast<int32>(heightMapResolution_);
    return (position.x >= 0 && position.y >= 0 && position.x < heightMapResolution && position.y < heightMapResolution);
}

double TerrainHeightGetter::GetHeightInWorldSpace(uint32 x, uint32 y) const
{
    return tools_.GetHeight(x, y) * terrainScale_.y;
}

double TerrainHeightGetter::GetHeightInTerrainQuad(const vec2ui& gridCoord, const vec2d& localPosition) const
{
    vec2d positionInQuad = GetPositionInQuad(localPosition);

    vec3d p3(0.0, GetHeightInWorldSpace(gridCoord.x, gridCoord.y + 1), gridSquereSize_.z);
    vec3d p1, p2;

    if (IsInLeftTriangle(positionInQuad))
    {
        p1 = vec3d(0.0, GetHeightInWorldSpace(gridCoord.x, gridCoord.y), 0.0);
        p2 = vec3d(gridSquereSize_.x, GetHeightInWorldSpace(gridCoord.x + 1, gridCoord.y), 0.0);
    }
    else
    {
        p1 = vec3d(gridSquereSize_.x, GetHeightInWorldSpace(gridCoord.x + 1, gridCoord.y), 0.0);
        p2 = vec3d(gridSquereSize_.x, GetHeightInWorldSpace(gridCoord.x + 1, gridCoord.y + 1), gridSquereSize_.z);
    }

    glm::dvec2 positionInQuadWorld = glm::dvec2(positionInQuad.x * gridSquereSize_.x, positionInQuad.y * gridSquereSize_.z);

    return Utils::BarryCentric(p1, p2, p3, positionInQuadWorld) + terrainPosition_.y;
}

vec3d TerrainHeightGetter::GetNormalInTerrainQuad(const vec2ui& gridCoord, const vec2d& localPosition) const
{
    auto positionInQuad = GetPositionInQuad(localPosition);

    if (IsInLeftTriangle(positionInQuad))
    {
        vec3d n1    = tools_.GetNormal(gridCoord.x, gridCoord.y);
        vec3d n2    = tools_.GetNormal(gridCoord.x + 1, gridCoord.y);
        vec3d n3    = tools_.GetNormal(gridCoord.x, gridCoord.y + 1);
        auto normal = n1 + n2 + n3 / 3.0;
        return glm::normalize(normal);
    }
    else
    {
        vec3d n1    = tools_.GetNormal(gridCoord.x + 1, gridCoord.y);
        vec3d n2    = tools_.GetNormal(gridCoord.x + 1, gridCoord.y + 1);
        vec3d n3    = tools_.GetNormal(gridCoord.x, gridCoord.y + 1);
        auto normal = n1 + n2 + n3 / 3.0;
        return glm::normalize(normal);
    }
}
}  // namespace GameEngine
