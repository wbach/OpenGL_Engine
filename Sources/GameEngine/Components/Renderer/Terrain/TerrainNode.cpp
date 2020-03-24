#include "TerrainNode.h"

namespace GameEngine
{
TerrainNode::TerrainNode(const TerrainConfiguration& terrainConfiguration, const vec2& location, int32 lod,
                         const vec2& index)
    : terrainConfiguration_(terrainConfiguration)
    , perNodeBuffer_(index, location, lod,
                     vec3(terrainConfiguration.GetScaleXZ() / -2.f, 0, terrainConfiguration.GetScaleXZ() / -2.f),
                     terrainConfiguration.GetScale(), terrainConfiguration.GetTerrainRootNodesCount())
    , worldPos_(0)
    , isleaf_(true)
    , halfGap_(perNodeBuffer_.gapAndLod.x / 2.f)
{
    ComputeWorldPosition();
    const vec3 cameraPos(0);
    Update(cameraPos);
}
void TerrainNode::Update(const vec3& cameraPosition)
{
    UpdateYPosition(cameraPosition);
    UpdateQuadTree(cameraPosition);
    UpdateChildren(cameraPosition);
}
const std::vector<std::unique_ptr<TerrainNode>>& TerrainNode::GetChildren() const
{
    return children_;
}

const PerNode& TerrainNode::GetPerNodeBuffer() const
{
    return perNodeBuffer_;
}

int32 TerrainNode::GetLod() const
{
    return static_cast<int32>(perNodeBuffer_.gapAndLod.y);
}
vec2 TerrainNode::GetLocation() const
{
    return vec2(perNodeBuffer_.indexAndLocation.z, perNodeBuffer_.indexAndLocation.w);
}

bool TerrainNode::IsLeaf() const
{
    return isleaf_;
}

void TerrainNode::ComputeWorldPosition()
{
    vec2 loc =
        (GetLocation() + halfGap_) * terrainConfiguration_.GetScaleXZ() - terrainConfiguration_.GetScaleXZ() / 2.f;
    worldPos_.x = loc.x;
    worldPos_.z = loc.y;
}
void TerrainNode::UpdateQuadTree(const vec3& cameraPosition)
{
    float distance = glm::length(cameraPosition - worldPos_);

    const auto& lodRange = terrainConfiguration_.GetLodRange(GetLod());

    if (distance < lodRange)
    {
        Divide();
    }
    else
    {
        RemoveChildren();
    }
}
void TerrainNode::Divide()
{
    if (isleaf_)
    {
        isleaf_ = false;
    }
    else
    {
        return;
    }

    if (children_.empty())
    {
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                const vec2 childLocation = GetLocation() + vec2(i * halfGap_, j * halfGap_);
                const vec2 index(i, j);
                children_.emplace_back(new TerrainNode(terrainConfiguration_, childLocation, GetLod() + 1, index));
            }
        }
    }
}

void TerrainNode::RemoveChildren()
{
    if (children_.empty())
    {
        return;
    }
    children_.clear();
    isleaf_ = true;
}

void TerrainNode::UpdateYPosition(const vec3& cameraPosition)
{
    if (cameraPosition.y > terrainConfiguration_.GetScaleY())
    {
        worldPos_.y = terrainConfiguration_.GetScaleY();
    }
    else
    {
        worldPos_.y = cameraPosition.y;
    }
}

void TerrainNode::UpdateChildren(const vec3& cameraPosition)
{
    for (auto& child : children_)
    {
        child->Update(cameraPosition);
    }
}
}  // namespace GameEngine
