#pragma once
#include <memory>
#include <vector>

#include "PerNodeBuffer.h"
#include "Types.h"

namespace GameEngine
{
class TerrainConfiguration;

class TerrainNode
{
public:
    TerrainNode(const TerrainConfiguration& terrainConfiguration, const vec2& location, int32 lod, const vec2& index);
    TerrainNode(const TerrainNode&&) = delete;
    TerrainNode(const TerrainNode&)  = delete;
    void Update(const vec3& cameraPosition);
    const std::vector<std::unique_ptr<TerrainNode>>& GetChildren() const;
    const PerNode& GetPerNodeBuffer() const;
    bool IsLeaf() const;

private:
    int32 GetLod() const;
    vec2 GetLocation() const;
    void ComputeWorldPosition();
    void UpdateQuadTree(const vec3& cameraPosition);
    void Divide();
    void RemoveChildren();
    void UpdateYPosition(const vec3& cameraPosition);
    void UpdateChildren(const vec3& cameraPosition);

private:
    const TerrainConfiguration& terrainConfiguration_;
    PerNode perNodeBuffer_;
    vec3 worldPos_;
    bool isleaf_;
    float halfGap_;
    std::vector<std::unique_ptr<TerrainNode>> children_;
};
}  // namespace GameEngine
