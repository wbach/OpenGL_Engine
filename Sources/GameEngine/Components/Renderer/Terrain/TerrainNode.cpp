#include "TerrainNode.h"

namespace GameEngine
{
TerrainNode::TerrainNode(const TerrainConfiguration& terrainConfiguration, const vec2& location, uint32 lod,
                         const vec2& index)
    : terrainConfiguration_(terrainConfiguration)
    , location_(location)
    , worldPos_(0)
    , index_(index)
    , lod_(lod)
    , isleaf_(true)
    , gap_(1.f / (static_cast<float>(TerrainDef::TERRAIN_ROOT_NODES_COUNT) * powf(2, lod)))
    , halfGap_(gap_ / 2.f)
{
    localTransform_.SetScale(vec3(gap_, 0, gap_));
    localTransform_.SetPosition(vec3(location_.x, 0, location_.y));
    localTransform_.TakeSnapShoot();

    vec3 scale(terrainConfiguration.GetScaleXZ(), terrainConfiguration.GetScaleY(),
                   terrainConfiguration.GetScaleXZ());

    vec3 worldPosition = scale / -2.f;
    worldPosition.y    = 0;

    worldTransform_.SetScale(scale);
    worldTransform_.SetPosition(worldPosition);
    worldTransform_.TakeSnapShoot();

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
float TerrainNode::GetGap() const
{
    return gap_;
}
uint32 TerrainNode::GetLod() const
{
    return lod_;
}
const vec2& TerrainNode::GetLocation() const
{
    return location_;
}
const vec2& TerrainNode::GetIndex() const
{
    return index_;
}
bool TerrainNode::IsLeaf() const
{
    return isleaf_;
}
const mat4& TerrainNode::GetWorldMatrix() const
{
    return worldTransform_.GetMatrix();
}
const mat4& TerrainNode::GetLocalMatrix() const
{
    return localTransform_.GetMatrix();
}
void TerrainNode::ComputeWorldPosition()
{
    vec2 loc    = (location_ + halfGap_) * terrainConfiguration_.GetScaleXZ() - terrainConfiguration_.GetScaleXZ() / 2.f;
    worldPos_.x = loc.x;
    worldPos_.z = loc.y;
}
void TerrainNode::UpdateQuadTree(const vec3& cameraPosition)
{
    float distance = glm::length(cameraPosition - worldPos_);

    const auto& lodRange = terrainConfiguration_.GetLodRange(lod_);

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
                const vec2 childLocation = location_ + vec2(i * halfGap_, j * halfGap_);
                const vec2 index(i, j);
                children_.emplace_back(new TerrainNode(terrainConfiguration_, childLocation, lod_ + 1, index));
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
