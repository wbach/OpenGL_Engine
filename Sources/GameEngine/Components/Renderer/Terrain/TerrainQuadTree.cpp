#include "TerrainQuadTree.h"

namespace GameEngine
{
TerrainQuadTree::TerrainQuadTree(const TerrainConfiguration& terrainConfiguration)
    : terrainConfiguration_(terrainConfiguration)
{
    constexpr uint32 lod           = 0;
    uint32 rootNodesCount = static_cast<uint32>(terrainConfiguration_.GetTerrainRootNodesCount());

    for (uint32 i = 0; i < rootNodesCount; ++i)
    {
        for (uint32 j = 0; j < rootNodesCount; ++j)
        {
            const auto _if = static_cast<float>(i);
            const auto _jf = static_cast<float>(j);

            const vec2 location(_if / terrainConfiguration_.GetTerrainRootNodesCount(), _jf / terrainConfiguration_.GetTerrainRootNodesCount());
            const vec2 index(_if, _jf);

            nodes_.emplace_back(new TerrainNode(terrainConfiguration_, location, lod, index));
        }
    }
}
const TerrainNodes& TerrainQuadTree::GetNodes() const
{
    return nodes_;
}
void TerrainQuadTree::Update(const vec3& cameraPosition)
{
    for (auto& node : nodes_)
    {
        node->Update(cameraPosition);
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
