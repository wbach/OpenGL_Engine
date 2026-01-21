#pragma once
#include <GameEngine/Resources/Models/Loaders/Binary/ModelReleatedSerializationData.h>

#include <filesystem>
#include <vector>

#include "Image/Image.h"
#include "Leaf.h"

namespace GameEngine
{
struct TreeClustersSerializationData
{
    TreeClusters clusters;
    std::vector<Utils::Image> baseColorTextures;
    std::vector<Utils::Image> normalsTextures;
};

struct LeafsSerializationData
{
    std::vector<LeafSSBO> allLeafs;
    MaterialSerilizeData material;
    TreeClustersSerializationData clusterData;
    ModelSerializeData bilboardModel;
};

struct TrunkSerializationData
{
    ModelSerializeData mainModel;
    ModelSerializeData lodModel;
};

struct TreeSerilizeData
{
    TrunkSerializationData trunk;
    LeafsSerializationData leafsData;
};
}  // namespace GameEngine
