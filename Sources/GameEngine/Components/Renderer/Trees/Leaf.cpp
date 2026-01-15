#include "Leaf.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/traits/vector.h>

#include <fstream>

#include "GameEngine/Resources/Models/Loaders/Binary/BinaryExporter.h"
#include "GameEngine/Resources/Models/Loaders/Binary/BinaryImporter.h"

namespace GameEngine
{
template <typename S>
void serialize(S& s, LeafSSBO& leaf)
{
    s.object(leaf.positionAndSizeRandomness.value);
    s.object(leaf.direction.value);
    s.object(leaf.colorRandomnessAndTextureIndex.value);
}

template <typename S>
void serialize(S& s, std::vector<LeafSSBO>& leafs)
{
    constexpr size_t MAX_SIZE = std::numeric_limits<size_t>::max();
    s.container(leafs, MAX_SIZE);
}

LeafSSBO ConvertToSSBO(const Leaf& leaf)
{
    LeafSSBO ssbo;
    ssbo.positionAndSizeRandomness      = glm::vec4(leaf.position, leaf.sizeRandomness);
    ssbo.direction                      = glm::vec4(leaf.direction, 0.0f);
    ssbo.colorRandomnessAndTextureIndex = glm::vec4(leaf.colorRandomness, static_cast<float>(leaf.textureIndex));

    return ssbo;
}
std::vector<LeafSSBO> PrepareSSBOData(const std::vector<Leaf>& cpuLeaves)
{
    std::vector<LeafSSBO> ssboData;
    ssboData.reserve(cpuLeaves.size());
    for (const auto& leaf : cpuLeaves)
    {
        ssboData.push_back(ConvertToSSBO(leaf));
    }
    return ssboData;
}
void ExportLeafSSBO(const std::vector<LeafSSBO>& leafs, const Material& material, const std::filesystem::path& path)
{
    auto outputPath = path;
    outputPath.replace_extension(".bin");

    std::ofstream file(outputPath, std::ios::binary);
    if (not file)
    {
        LOG_WARN << "Open file error: " << outputPath;
        return;
    }

    auto materialSerilizeData = convert(material);
    bitsery::Serializer<bitsery::OutputStreamAdapter> ser{file};
    ser.object(leafs);
    ser.object(materialSerilizeData);

    if (not file.good())
    {
        LOG_ERROR << "File write error: " << outputPath;
        return;
    }

    LOG_DEBUG << "Export done: " << outputPath;
}
std::pair<std::vector<LeafSSBO>, Material> ImportLeafSSBO(ITextureLoader& textureLoader, const std::filesystem::path& path)
{
    std::vector<LeafSSBO> result;

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        LOG_ERROR << "Failed to open binary model file: " << path;
        return {};
    }

    MaterialSerilizeData materialSerilizeData;
    bitsery::Deserializer<bitsery::InputStreamAdapter> des{file};
    des.object(result);
    des.object(materialSerilizeData);

    if (not file.good())
    {
        LOG_ERROR << "Binary model import failed: " << path;
        return {};
    }

    return {result, convert(textureLoader, materialSerilizeData)};
}
std::ostream& operator<<(std::ostream& os, const Cluster& cluster)
{
    os << "Cluster[Leaves: " << cluster.leafIndices.size() << ", Min: (" << cluster.minBound.x << ", " << cluster.minBound.y
       << ", " << cluster.minBound.z << ")"
       << ", Max: (" << cluster.maxBound.x << ", " << cluster.maxBound.y << ", " << cluster.maxBound.z << ")]";
    return os;
}
std::ostream& operator<<(std::ostream& os, const TreeClusters& tc)
{
    os << "=== TreeClusters Statistics ===\n"
       << "Grid Origin:  (" << tc.gridOrigin.x << ", " << tc.gridOrigin.y << ", " << tc.gridOrigin.z << ")\n"
       << "Voxel Size:   (" << tc.voxelSize.x << ", " << tc.voxelSize.y << ", " << tc.voxelSize.z << ")\n"
       << "Grid Size:    [" << tc.gridSize.x << "x" << tc.gridSize.y << "x" << tc.gridSize.z << "]\n"
       << "Total Clusters: " << tc.clusters.size() << "\n";

    if (not tc.clusters.empty())
    {
        os << "Clusters Details:\n";
        for (size_t i = 0; i < tc.clusters.size(); ++i)
        {
            os << "  [" << std::setw(3) << i << "] Leaf Count: " << tc.clusters[i].leafIndices.size() << "\n";
        }
    }
    os << "================================";
    return os;
}
}  // namespace GameEngine
