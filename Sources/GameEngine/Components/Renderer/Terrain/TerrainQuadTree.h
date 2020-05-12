#pragma once
#include "TerrainConfiguration.h"
#include "TerrainNode.h"

namespace GameEngine
{
typedef std::unique_ptr<TerrainNode> TerrainNodePtr;
typedef std::vector<TerrainNodePtr> TerrainNodes;

class TerrainQuadTree
{
public:
    TerrainQuadTree(const TerrainConfiguration& terrainConfiguration);
    TerrainQuadTree(const TerrainQuadTree&)  = delete;
    TerrainQuadTree(const TerrainQuadTree&&) = delete;
    void CreateNodes();
    const TerrainNodes& GetNodes() const;
    void Update(const vec3& cameraPosition);

private:
    std::vector<vec2> GeneratePatch();

private:
    TerrainNodes nodes_;

private:
    const TerrainConfiguration& terrainConfiguration_;
};
}  // namespace GameEngine
