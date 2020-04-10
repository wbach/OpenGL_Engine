#include "TerrainHeightGetter.h"
#include <algorithm>
#include "GLM/GLMUtils.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
namespace Components
{
TerrainHeightGetter::TerrainHeightGetter(const TerrainConfiguration& terrainConfiguration, const HeightMap& heightMap,
                                         const vec2& terrainPosition)
    : terrainConfiguration_(terrainConfiguration)
    , heightMap_(heightMap)
    , terrainPosition_(terrainPosition)
    , yOffset_(-1.f * heightMap.GetMaximumHeight() / 2.f * terrainConfiguration.GetScale().y)
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

    return GetHeightInTerrainQuad(gridCoord, localPosition) + yOffset_;
}

vec2 TerrainHeightGetter::GetLocalPositionOnTerrain(float worldX, float worldZ) const
{
    float terrain_x = worldX - terrainPosition_.x;
    float terrain_z = worldZ - terrainPosition_.y;

    return vec2(terrain_x, terrain_z);
}

vec2i TerrainHeightGetter::GetGridCoord(const vec2& position) const
{
    int x = static_cast<int>(floorf(position.x / gridSquereSize_));
    int y = static_cast<int>(floorf(position.y / gridSquereSize_));

    return vec2i(x, y);
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

bool TerrainHeightGetter::IsValidGridCoordinate(const vec2i& position) const
{
    return not(position.x >= heightMapResolution_ - 1 || position.y >= heightMapResolution_ - 1 || position.x < 0 ||
               position.y < 0);
}

float TerrainHeightGetter::GetHeightInTerrainQuad(const vec2i& gridCoord, const vec2& localPosition) const
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

float TerrainHeightGetter::GetHeight(int x, int y) const
{
    return heightMap_.GetImage().floatData[static_cast<size_t>(x + y * heightMapResolution_)];
}

}  // namespace Components
}  // namespace GameEngine
