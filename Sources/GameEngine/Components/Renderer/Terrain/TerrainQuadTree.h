#pragma once
#include "TerrainConfiguration.h"
#include "TerrainNode.h"

namespace GameEngine
{
class TerrainQuadTree
{
public: 
    TerrainQuadTree(const TerrainConfiguration& terrainConfiguration);
    TerrainQuadTree(const TerrainQuadTree&) = delete;
    TerrainQuadTree(const TerrainQuadTree&&) = delete;
    const std::vector<TerrainNode>& GetNodes() const;
    void Update(const vec3& cameraPosition);

private:
    std::vector<vec2> GeneratePatch();

private:
    std::vector<TerrainNode> nodes_;

private:
    const TerrainConfiguration& terrainConfiguration_;
};
}  // namespace GameEngine
