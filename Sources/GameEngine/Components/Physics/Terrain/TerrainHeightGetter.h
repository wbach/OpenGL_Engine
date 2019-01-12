#pragma once
#include <vector>
#include <optional>
#include "Types.h"

namespace GameEngine
{
namespace Components
{
class TerrainHeightGetter
{
public:
    TerrainHeightGetter(const vec2ui& size, std::vector<float>* heights, const vec2& terrainPosition);
    std::optional<float> GetHeightofTerrain(const vec2& posXZ);
    std::optional<float> GetHeightofTerrain(float worldX, float worldZ);

private:
    float GetHeight(int x, int y) const;
    vec2i GetGridCoord(const vec2& position) const;
    vec2 GetPositionInQuad(const vec2& position) const;
    bool IsInLeftTriangle(const vec2& position) const;
    bool IsNotValidGridCoordinate(const vec2i& position) const;
    vec2 GetLocalPositionOnTerrain(float worldX, float worldZ);
    float GetHeightInTerrainQuad(const vec2i& gridCoord, const vec2& localPosition) const;

private:
    vec2ui size_;
    float gridSquereSize_;
    vec2 terrainPosition_;
    int heightMapResolution_;
    std::vector<float>* data_;

};
}  // namespace Components
}  // namespace GameEngine
