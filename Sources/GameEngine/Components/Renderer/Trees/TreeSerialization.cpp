#include "TreeSerialization.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>
#include <Utils/Image/ImageSerializer.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/traits/vector.h>

#include <fstream>
#include <memory>
#include <vector>

#include "FileSystem/FileSystemUtils.hpp"
#include "GameEngine/Components/Renderer/Trees/Leaf.h"
#include "GameEngine/Components/Renderer/Trees/TreeSerializationData.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Models/Loaders/Binary/BinaryExporter.h"
#include "GameEngine/Resources/Models/Loaders/Binary/BinaryImporter.h"
#include "GameEngine/Resources/Models/Loaders/Binary/BinarySerializeHelpers.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GraphicsApi/TextureParamters.h"
#include "Image/ImageUtils.h"

namespace GameEngine
{
template <typename S>
void serialize(S& s, TrunkSerializationData& m)
{
    s.object(m.mainModel);
    s.object(m.lodModel);
}

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

template <typename S>
void serialize(S& s, Cluster& m)
{
    s.object(m.minBound);
    s.object(m.maxBound);

    constexpr size_t MAX_SIZE = std::numeric_limits<size_t>::max();
    s.container4b(m.leafIndices, MAX_SIZE);
}

template <typename S>
void serialize(S& s, std::vector<Cluster>& m)
{
    constexpr size_t MAX_SIZE = std::numeric_limits<size_t>::max();
    s.container(m, MAX_SIZE);
}

template <typename S>
void serialize(S& s, TreeClusters& m)
{
    s.object(m.gridOrigin);
    s.object(m.voxelSize);
    s.object(m.gridSize);
    s.object(m.clusters);
}

template <typename S>
void serialize(S& s, TreeClustersSerializationData& m)
{
    s.object(m.clusters);
    s.object(m.baseColorTextures);
    s.object(m.normalsTextures);
}

template <typename S>
void serialize(S& s, LeafsSerializationData& m)
{
    s.object(m.allLeafs);
    s.object(m.material);
    s.object(m.clusterData);
    s.object(m.bilboardModel);
}

void ExportTree(const TreeSerilizeData& data, const std::filesystem::path& path)
{
    auto outputPath = path;
    outputPath.replace_extension(".bin");

    std::ofstream file(outputPath, std::ios::binary);
    if (not file)
    {
        LOG_WARN << "Open file error: " << outputPath;
        return;
    }
    bitsery::Serializer<bitsery::OutputStreamAdapter> ser{file};
    ser.object(data.leafsData);
    ser.object(data.trunk);

    if (not file.good())
    {
        LOG_ERROR << "File write error: " << outputPath;
        return;
    }

    LOG_DEBUG << "Export done: " << outputPath;
}
TreeSerilizeData ImportTree(const std::filesystem::path& path)
{
    TreeSerilizeData result;

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        LOG_ERROR << "Failed to open binary model file: " << path;
        return {};
    }

    bitsery::Deserializer<bitsery::InputStreamAdapter> des{file};
    des.object(result.leafsData);
    des.object(result.trunk);

    if (not file.good())
    {
        LOG_ERROR << "Binary model import failed: " << path;
        return {};
    }

    return result;
}

TreeClustersSerializationData convert(const Tree::Clusters& clusters)
{
    TreeClustersSerializationData result;
    if (clusters.clusterTextures.baseColorTexture)
    {
        result.baseColorTextures = clusters.clusterTextures.baseColorTexture->GetImages();
        Utils::CompressImagesParallel(result.baseColorTextures);
    }

    if (clusters.clusterTextures.normalTexture)
    {
        result.normalsTextures = clusters.clusterTextures.normalTexture->GetImages();
        Utils::CompressImagesParallel(result.normalsTextures);
    }

    result.clusters = clusters.treeClusters;
    return result;
}

LeafsSerializationData convert(const std::vector<LeafSSBO>& leafsSsbo, const Tree::Clusters& clusters,
                               const Model* leafBilboardsModel, const Material& leafMaterial)
{
    LeafsSerializationData result;
    result.allLeafs    = leafsSsbo;
    result.material    = convert(leafMaterial);
    result.clusterData = convert(clusters);
    if (leafBilboardsModel)
    {
        result.bilboardModel = convert(*leafBilboardsModel);
    }
    return result;
}

TrunkSerializationData convert(const ModelWrapper& trunkModel)
{
    TrunkSerializationData result;
    if (auto lodModel = trunkModel.Get(LevelOfDetail::L2))
    {
        result.lodModel = convert(*lodModel);
    }
    if (auto model = trunkModel.Get(LevelOfDetail::L1))
    {
        result.mainModel = convert(*model);
    }
    return result;
}

TreeSerilizeData convert(const Tree& tree)
{
    auto leafsData = convert(tree.allLeafs, tree.clusters, tree.leafBilboardsModel, tree.leafMaterial);
    auto trunk     = convert(tree.trunkModel);
    return TreeSerilizeData{.trunk = std::move(trunk), .leafsData = std::move(leafsData)};
}

Tree::Clusters convert(TreeClustersSerializationData&& data, ITextureLoader& textureLoader)
{
    Tree::Clusters result;
    TextureParameters paramters{.dataStorePolicy = DataStorePolicy::Store,
                                .filter          = GraphicsApi::TextureFilter::LINEAR,
                                .mimap           = GraphicsApi::TextureMipmap::LINEAR};

    Utils::DecompressImagesParallel(data.baseColorTextures);
    Utils::DecompressImagesParallel(data.normalsTextures);

    result.clusterTextures.baseColorTexture =
        textureLoader.CreateTexture(Utils::CreateUniqueFilename(), paramters, std::move(data.baseColorTextures));

    result.clusterTextures.normalTexture =
        textureLoader.CreateTexture(Utils::CreateUniqueFilename(), paramters, std::move(data.normalsTextures));

    result.treeClusters = std::move(data.clusters);
    return result;
}

Tree Convert(TreeSerilizeData& data, GraphicsApi::IGraphicsApi& graphicsApi, IResourceManager& resourceManager)
{
    Tree result;

    auto& textureLoader = resourceManager.GetTextureLoader();
    auto trunkLodModel  = convert(std::move(data.trunk.lodModel), graphicsApi, textureLoader);
    auto trunkModel     = convert(std::move(data.trunk.mainModel), graphicsApi, textureLoader);

    if (trunkModel)
    {
        result.trunkModel.Add(trunkModel.get(), LevelOfDetail::L1);
        resourceManager.AddModel(std::move(trunkModel));
    }

    if (trunkLodModel)
    {
        result.trunkModel.Add(trunkLodModel.get(), LevelOfDetail::L2);
        resourceManager.AddModel(std::move(trunkLodModel));
    }

    auto leafBilboardsModel = convert(std::move(data.leafsData.bilboardModel), graphicsApi, textureLoader);

    if (leafBilboardsModel)
    {
        result.leafBilboardsModel = leafBilboardsModel.get();
        resourceManager.AddModel(std::move(leafBilboardsModel));
    }

    result.clusters = convert(std::move(data.leafsData.clusterData), resourceManager.GetTextureLoader());

    result.leafMaterial = convert(textureLoader, data.leafsData.material);
    result.allLeafs     = std::move(data.leafsData.allLeafs);

    return result;
}
void Export(const Tree& tree, const std::filesystem::path& path)
{
    LOG_DEBUG << "Export tree to file: " << path;
    auto treeData = convert(tree);
    ExportTree(treeData, path);
}
Tree Import(GraphicsApi::IGraphicsApi& graphicsApi, IResourceManager& resourceManager, const std::filesystem::path& path)
{
    LOG_DEBUG << "Import tree from file: " << path;
    auto treeData = ImportTree(path);
    return Convert(treeData, graphicsApi, resourceManager);
}
}  // namespace GameEngine
