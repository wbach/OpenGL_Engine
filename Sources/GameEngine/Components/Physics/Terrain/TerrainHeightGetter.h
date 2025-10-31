#pragma once
#include <optional>
#include <vector>

#include "GameEngine/Components/Renderer/Terrain/TerrainHeightTools.h"
#include "Types.h"

namespace GameEngine
{
class HeightMap;
class TerrainConfiguration;

namespace Components
{
class TerrainRendererComponent;
}

class TerrainHeightGetter
{
public:
    TerrainHeightGetter(Components::TerrainRendererComponent&);
    TerrainHeightGetter(const vec3&, const HeightMap&, const vec3& terrainPosition);
    std::optional<float> GetHeightofTerrain(const vec2& worldPositionXZ) const;
    std::optional<double> GetHeightofTerrain(const vec2d& worldPositionXZ) const;
    std::optional<float> GetHeightofTerrain(float worldX, float worldZ) const;
    std::optional<vec3> GetPointOnTerrain(float worldX, float worldZ) const;
    std::optional<vec3> GetNormalOfTerrain(float worldX, float worldZ) const;

private:
    std::optional<vec2ui> GetGridCoord(const vec2d& position) const;
    vec2d GetPositionInQuad(const vec2d& position) const;
    bool IsInLeftTriangle(const vec2d& position) const;
    bool IsValidGridCoordinate(const vec2i& position) const;
    std::optional<vec2d> GetLocalPositionOnTerrain(const vec2d&) const;
    double GetHeightInTerrainQuad(const vec2ui& gridCoord, const vec2d& localPosition) const;
    vec3d GetNormalInTerrainQuad(const vec2ui& gridCoord, const vec2d& localPosition) const;
    double GetHeightInWorldSpace(uint32 x, uint32 y) const;

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
