#include "TerrainQuadTree.h"
#include "TerrainDef.h"

namespace GameEngine
{
TerrainQuadTree::TerrainQuadTree(const TerrainConfiguration& terrainConfiguration)
    : terrainConfiguration_(terrainConfiguration)
{
    constexpr uint32 lod           = 0;
    constexpr float rootNodesCount = static_cast<float>(TerrainDef::TERRAIN_ROOT_NODES_COUNT);

    for (int i = 0; i < TerrainDef::TERRAIN_ROOT_NODES_COUNT; ++i)
    {
        for (int j = 0; j < TerrainDef::TERRAIN_ROOT_NODES_COUNT; ++j)
        {
            const auto _if = static_cast<float>(i);
            const auto _jf = static_cast<float>(j);

            const vec2 location(_if / rootNodesCount, _jf / rootNodesCount);
            const vec2 index(_if, _jf);

            nodes_.emplace_back(terrainConfiguration_, location, lod, index);
        }
    }
}
const std::vector<TerrainNode>& TerrainQuadTree::GetNodes() const
{
    return nodes_;
}
void TerrainQuadTree::Update(const vec3& cameraPosition)
{
    for (auto& node : nodes_)
    {
        node.Update(cameraPosition);
    }
}
std::vector<vec2> TerrainQuadTree::GeneratePatch()
{
    // clang-format off
    return
    {
        vec2(0, 0),
        vec2(0.333f, 0),
        vec2(0.666f, 0),
        vec2(1, 0),

        vec2(0, 0.333f),
        vec2(0.333f, 0.333f),
        vec2(0.666f, 0.333f),
        vec2(1, 0.333f),

        vec2(0, 0.666f),
        vec2(0.333f, 0.666f),
        vec2(0.666f, 0.666f),
        vec2(1, 0.666f),

        vec2(0, 1),
        vec2(0.333f, 1),
        vec2(0.666f, 1),
        vec2(1, 1)
    };
    // clang-format on
}
}  // namespace GameEngine
