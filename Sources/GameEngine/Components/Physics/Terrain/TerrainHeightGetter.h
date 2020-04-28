#pragma once
#include <vector>
#include <optional>
#include "Types.h"

namespace GameEngine
{
class HeightMap;
class TerrainConfiguration;

class TerrainHeightGetter
{
public:
    TerrainHeightGetter(const TerrainConfiguration&, const HeightMap&, const vec2 &terrainPosition);
    std::optional<float> GetHeightofTerrain(const vec2& posXZ) const;
    std::optional<float> GetHeightofTerrain(float worldX, float worldZ) const;

private:
    float GetHeight(uint32 x, uint32 y) const;
    vec2ui GetGridCoord(const vec2& position) const;
    vec2 GetPositionInQuad(const vec2& position) const;
    bool IsInLeftTriangle(const vec2& position) const;
    bool IsValidGridCoordinate(const vec2ui &position) const;
    vec2 GetLocalPositionOnTerrain(float worldX, float worldZ) const;
    float GetHeightInTerrainQuad(const vec2ui &gridCoord, const vec2& localPosition) const;

private:
    const TerrainConfiguration& terrainConfiguration_;
    const HeightMap& heightMap_;
    vec2 terrainPosition_;

private:
    uint32 heightMapResolution_;
    float gridSquereSize_;
    float yOffset_;

};
}  // namespace GameEngine
