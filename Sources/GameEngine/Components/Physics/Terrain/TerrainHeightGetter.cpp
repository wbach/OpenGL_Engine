#include "TerrainHeightGetter.h"
#include <algorithm>
#include "GLM/GLMUtils.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainUtils.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include <Logger/Log.h>

namespace GameEngine
{
namespace Components
{
TerrainHeightGetter::TerrainHeightGetter(const TerrainConfiguration& terrainConfiguration, const HeightMap& heightMap,
                                         const vec2& terrainPosition)
    : terrainConfiguration_(terrainConfiguration)
    , heightMap_(heightMap)
    , terrainPosition_(terrainPosition)
    , yOffset_(heightMap.GetMaximumHeight() / 2.f * terrainConfiguration.GetScale().y)
{
    heightMapResolution_ = heightMap_.GetImage().width;
    gridSquereSize_      = terrainConfiguration_.GetScale().x / (heightMapResolution_ - 1.f);
}

std::optional<float> TerrainHeightGetter::GetHeightofTerrain(const vec2& posXZ) const
{
    return GetHeightofTerrain(posXZ.x, posXZ.y);
}

std::optional<float> TerrainHeightGetter::GetHeightofTerrain(float worldX, float worldZ) const
{
    auto localPosition = GetLocalPositionOnTerrain(worldX, worldZ);

    auto gridCoord = GetGridCoord(localPosition + vec2(terrainConfiguration_.GetScale().x / 2.f));

    if (not IsValidGridCoordinate(gridCoord))
        return std::nullopt;

    return GetHeightInTerrainQuad(gridCoord, localPosition);
}

vec2 TerrainHeightGetter::GetLocalPositionOnTerrain(float worldX, float worldZ) const
{
    float terrain_x = worldX - terrainPosition_.x;
    float terrain_z = worldZ - terrainPosition_.y;

    return vec2(terrain_x, terrain_z);
}

vec2ui TerrainHeightGetter::GetGridCoord(const vec2& position) const
{
    uint32 x = static_cast<uint32>(floorf(position.x / gridSquereSize_));
    uint32 y = static_cast<uint32>(floorf(position.y / gridSquereSize_));

    return vec2ui(x, y);
}

vec2 TerrainHeightGetter::GetPositionInQuad(const vec2& position) const
{
    float x_coord = (fmodf(position.x, gridSquereSize_)) / gridSquereSize_;
    float z_coord = (fmodf(position.y, gridSquereSize_)) / gridSquereSize_;

    return vec2(x_coord, z_coord);
}

bool TerrainHeightGetter::IsInLeftTriangle(const vec2& position) const
{
    return position.x <= (1 - position.y);
}

bool TerrainHeightGetter::IsValidGridCoordinate(const vec2ui& position) const
{
    return (position.x < heightMapResolution_ or position.y < heightMapResolution_);
}

float TerrainHeightGetter::GetHeightInTerrainQuad(const vec2ui& gridCoord, const vec2& localPosition) const
{
    auto positionInQuad = GetPositionInQuad(localPosition);

    vec3 p3(0, GetHeight(gridCoord.x, gridCoord.y + 1), 1);
    vec3 p1, p2;

    if (IsInLeftTriangle(positionInQuad))
    {
        p1 = vec3(0, GetHeight(gridCoord.x, gridCoord.y), 0);
        p2 = vec3(1, GetHeight(gridCoord.x + 1, gridCoord.y), 0);
    }
    else
    {
        p1 = vec3(1, GetHeight(gridCoord.x + 1, gridCoord.y), 0);
        p2 = vec3(1, GetHeight(gridCoord.x + 1, gridCoord.y + 1), 1);
    }

    return Utils::BarryCentric(p1, p2, p3, positionInQuad);
}

float TerrainHeightGetter::GetHeight(uint32 x, uint32 y) const
{
    return GetTerrainHeight(heightMap_.GetImage().floatData, terrainConfiguration_.GetScale().y, heightMapResolution_, yOffset_, x, y);
}

}  // namespace Components
}  // namespace GameEngine
