#pragma once
#include <math.hpp>
#include <memory>
#include <vector>
#include "Common/Transform.h"
#include "TerrainConfiguration.h"
#include "TerrainDef.h"
#include "Types.h"

namespace GameEngine
{
class TerrainNode
{
public:
    TerrainNode(const TerrainConfiguration& terrainConfiguration, const vec2& location, uint32 lod, const vec2& index);
    TerrainNode(const TerrainNode&&) = delete;
    TerrainNode(const TerrainNode&) = delete;
    void Update(const vec3& cameraPosition);
    const std::vector<std::unique_ptr<TerrainNode>>& GetChildren() const;
    float GetGap() const;
    uint32 GetLod() const;
    const vec2& GetLocation() const;
    const vec2& GetIndex() const;
    bool IsLeaf() const;
    const mat4& GetWorldMatrix() const;
    const mat4& GetLocalMatrix() const;

private:
    void ComputeWorldPosition();
    void UpdateQuadTree(const vec3& cameraPosition);
    void Divide();
    void RemoveChildren();
    void UpdateYPosition(const vec3& cameraPosition);
    void UpdateChildren(const vec3& cameraPosition);

private:
    const TerrainConfiguration& terrainConfiguration_;
    vec2 location_;
    vec3 worldPos_;
    vec2 index_;
    uint32 lod_;
    bool isleaf_;
    float gap_;
    float halfGap_;
    common::Transform worldTransform_;
    common::Transform localTransform_;
    std::vector<std::unique_ptr<TerrainNode>> children_;
};
}  // namespace GameEngine
