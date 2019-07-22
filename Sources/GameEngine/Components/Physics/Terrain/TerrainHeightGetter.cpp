#include "TerrainHeightGetter.h"
#include "GLM/GLMUtils.h"
#include  <algorithm>

namespace GameEngine
{
namespace Components
{
TerrainHeightGetter::TerrainHeightGetter(const vec2ui& size, std::vector<float>* heights, const vec2& terrainPosition)
    : size_(size)
    , gridSquereSize_(1.f)
    , terrainPosition_(terrainPosition)
    , heightMapResolution_(static_cast<int>(size.x))
    , data_(heights)
    , yOffset_(0)
{
    //auto maxElementIter = std::max_element(heights->begin(), heights->end());
    //auto maxElement = maxElementIter != heights->end() ? *maxElementIter : 0.f;

    yOffset_ = -1.75f;// -maxElement / .2f;
}

std::optional<float> TerrainHeightGetter::GetHeightofTerrain(const vec2& posXZ)
{
    return GetHeightofTerrain(posXZ.x, posXZ.y);
}

std::optional<float> TerrainHeightGetter::GetHeightofTerrain(float worldX, float worldZ)
{
    auto localPosition = GetLocalPositionOnTerrain(worldX, worldZ);

    auto gridCoord = GetGridCoord(localPosition + vec2(size_.x / 2.f));

    if (not IsValidGridCoordinate(gridCoord))
        return std::nullopt;

    return GetHeightInTerrainQuad(gridCoord, localPosition) + yOffset_ ;
}

vec2 TerrainHeightGetter::GetLocalPositionOnTerrain(float worldX, float worldZ)
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
    return not (position.x >= heightMapResolution_ - 1 || position.y >= heightMapResolution_ - 1 || position.x < 0 ||
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

    return (*data_)[static_cast<size_t>(x + y * heightMapResolution_)];
}

}  // namespace Components
}  // namespace GameEngine
