#include "TerrainHeightGetter.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainDef.h"
#include "GLM/GLMUtils.h"

namespace GameEngine
{
namespace Components
{
TerrainHeightGetter::TerrainHeightGetter(const vec2ui& size, std::vector<float>* heights, const vec2& terrainPosition)
    : size_(size)
    , gridSquereSize_(TerrainDef::TOTAL_SIZE * TerrainDef::PART_SIZE / ((float)size.y - 1))
    , terrainPosition_(terrainPosition)
    , heightMapResolution_(0)
    , data_(heights)
{

}

std::optional<float> TerrainHeightGetter::GetHeightofTerrain(const vec2& posXZ)
{
    return GetHeightofTerrain(posXZ.x, posXZ.y);
}

std::optional<float> TerrainHeightGetter::GetHeightofTerrain(float worldX, float worldZ)
{
    auto localPosition = GetLocalPositionOnTerrain(worldX, worldZ);

    auto gridCoord = GetGridCoord(localPosition + vec2(TerrainDef::HALF_TOTAL_SIZE));

    if (IsNotValidGridCoordinate(gridCoord))
        return {};

    return GetHeightInTerrainQuad(gridCoord, localPosition);
}

vec2 TerrainHeightGetter::GetLocalPositionOnTerrain(float worldX, float worldZ)
{
    float terrain_x = worldX - terrainPosition_.x;
    float terrain_z = worldZ - terrainPosition_.y;

    return vec2(terrain_x, terrain_z);
}

vec2i TerrainHeightGetter::GetGridCoord(const vec2& position) const
{
    int x = (int)floor(position.x / gridSquereSize_);
    int y = (int)floor(position.y / gridSquereSize_);

    return vec2i(x, y);
}

vec2 TerrainHeightGetter::GetPositionInQuad(const vec2& position) const
{
    float x_coord = (fmod(position.x, gridSquereSize_)) / gridSquereSize_;
    float z_coord = (fmod(position.y, gridSquereSize_)) / gridSquereSize_;

    return vec2(x_coord, z_coord);
}

bool TerrainHeightGetter::IsInLeftTriangle(const vec2& position) const
{
    return position.x <= (1 - position.y);
}

bool TerrainHeightGetter::IsNotValidGridCoordinate(const vec2i& position) const
{
    return (position.x >= heightMapResolution_ - 1 || position.y >= heightMapResolution_ - 1 || position.x < 0 ||
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
    if ( not data_  or data_->empty())
        return 0;

    return (*data_)[x + y * heightMapResolution_];
}

}  // namespace Components
}  // namespace GameEngine
