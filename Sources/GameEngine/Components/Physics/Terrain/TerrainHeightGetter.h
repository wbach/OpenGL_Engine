#pragma once
#include <optional>
#include <vector>
#include "Types.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainHeightTools.h"

namespace GameEngine
{
class HeightMap;
class TerrainConfiguration;

class TerrainHeightGetter
{
public:
    TerrainHeightGetter(const vec3&, const HeightMap&, const vec3& terrainPosition);
    std::optional<float> GetHeightofTerrain(const vec2& worldPositionXZ) const;
    std::optional<float> GetHeightofTerrain(float worldX, float worldZ) const;
    std::optional<vec3> GetPointOnTerrain(float worldX, float worldZ) const;
    std::optional<vec3> GetNormalOfTerrain(float worldX, float worldZ) const;

private:
    std::optional<vec2ui> GetGridCoord(const vec2& position) const;
    vec2 GetPositionInQuad(const vec2& position) const;
    bool IsInLeftTriangle(const vec2& position) const;
    bool IsValidGridCoordinate(const vec2i &position) const;
    std::optional<vec2> GetLocalPositionOnTerrain(const vec2&) const;
    float GetHeightInTerrainQuad(const vec2ui& gridCoord, const vec2& localPosition) const;
    vec3 GetNormalInTerrainQuad(const vec2ui& gridCoord, const vec2& localPosition) const;

private:
    const vec3 terrainScale_;
    const HeightMap& heightMap_;
    vec3 terrainPosition_;

private:
    uint32 heightMapResolution_;
    vec3 gridSquereSize_;

    TerrainHeightTools tools_;
};
}  // namespace GameEngine
