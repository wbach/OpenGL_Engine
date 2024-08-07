#include "TerrainHeightGetter.h"
#include <Logger/Log.h>
#include <algorithm>
#include "GLM/GLMUtils.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
TerrainHeightGetter::TerrainHeightGetter(const vec3& terrainScale, const HeightMap& heightMap,
                                         const vec3& terrainPosition)
    : terrainScale_(terrainScale)
    , heightMap_(heightMap)
    , terrainPosition_(terrainPosition)
    , tools_(terrainScale, heightMap.GetImage())
{
    heightMapResolution_ = heightMap_.GetImage().width;
    gridSquereSize_      = terrainScale / static_cast<float>(heightMapResolution_ - 1.f);
}

std::optional<float> TerrainHeightGetter::GetHeightofTerrain(float worldX, float worldZ) const
{
    return GetHeightofTerrain(vec2(worldX, worldZ));
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
    auto localPosition = GetLocalPositionOnTerrain(vec2(worldX, worldZ));

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

std::optional<vec2> TerrainHeightGetter::GetLocalPositionOnTerrain(const vec2& worldPostion) const
{
    auto halfTerrainScale = terrainScale_ / 2.f;
    auto terrain_x = worldPostion.x + halfTerrainScale.x - terrainPosition_.x;
    auto terrain_z = worldPostion.y + halfTerrainScale.z - terrainPosition_.z;

    if (terrain_x > terrainScale_.x or terrain_z > terrainScale_.z)
        return std::nullopt;

    return vec2(terrain_x, terrain_z);
}

std::optional<vec2ui> TerrainHeightGetter::GetGridCoord(const vec2& position) const
{
    auto xi = static_cast<int32>(floorf(position.x / gridSquereSize_.x));
    auto yi = static_cast<int32>(floorf(position.y / gridSquereSize_.z));

    if (not IsValidGridCoordinate(vec2i(xi, yi)))
        return std::nullopt;

    return vec2ui(static_cast<uint32>(xi), static_cast<uint32>(yi));
}

vec2 TerrainHeightGetter::GetPositionInQuad(const vec2& position) const
{
    float x_coord = (fmodf(position.x, gridSquereSize_.x)) / gridSquereSize_.x;
    float z_coord = (fmodf(position.y, gridSquereSize_.z)) / gridSquereSize_.z;

    return vec2(x_coord, z_coord);
}

bool TerrainHeightGetter::IsInLeftTriangle(const vec2& position) const
{
    return position.x <= (1 - position.y);
}

bool TerrainHeightGetter::IsValidGridCoordinate(const vec2i& position) const
{
    auto heightMapResolution = static_cast<int32>(heightMapResolution_);

    return (position.x >= 0 and position.y >= 0 and position.x < heightMapResolution and
            position.y < heightMapResolution);
}

float TerrainHeightGetter::GetHeightInTerrainQuad(const vec2ui& gridCoord, const vec2& localPosition) const
{
    auto positionInQuad = GetPositionInQuad(localPosition);

    vec3 p3(0, tools_.GetHeight(gridCoord.x, gridCoord.y + 1), gridSquereSize_.z);
    vec3 p1, p2;

    if (IsInLeftTriangle(positionInQuad))
    {
        p1 = vec3(0, tools_.GetHeight(gridCoord.x, gridCoord.y), 0);
        p2 = vec3(gridSquereSize_.x, tools_.GetHeight(gridCoord.x + 1, gridCoord.y), 0);
    }
    else
    {
        p1 = vec3(gridSquereSize_.x, tools_.GetHeight(gridCoord.x + 1, gridCoord.y), 0);
        p2 = vec3(gridSquereSize_.x, tools_.GetHeight(gridCoord.x + 1, gridCoord.y + 1), gridSquereSize_.z);
    }

    return Utils::BarryCentric(p1, p2, p3, positionInQuad) * terrainScale_.y;
}

vec3 TerrainHeightGetter::GetNormalInTerrainQuad(const vec2ui& gridCoord, const vec2& localPosition) const
{
    auto positionInQuad = GetPositionInQuad(localPosition);

    if (IsInLeftTriangle(positionInQuad))
    {
        auto n1 = tools_.GetNormal(gridCoord.x, gridCoord.y);
        auto n2 = tools_.GetNormal(gridCoord.x + 1, gridCoord.y);
        auto n3 = tools_.GetNormal(gridCoord.x, gridCoord.y + 1);
        auto normal = n1 + n2 + n3 / 3.f;
        return glm::normalize(normal);
    }
    else
    {
        auto n1 = tools_.GetNormal(gridCoord.x + 1, gridCoord.y);
        auto n2 = tools_.GetNormal(gridCoord.x + 1, gridCoord.y + 1);
        auto n3 = tools_.GetNormal(gridCoord.x, gridCoord.y + 1);
        auto normal = n1 + n2 + n3 / 3.f;
        return glm::normalize(normal);
    }
}
}  // namespace GameEngine
