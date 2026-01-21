#pragma once
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderStorageVectorBufferObject.h"
#include "Leaf.h"

namespace GameEngine
{
struct Tree
{
    struct Clusters
    {
        ClusterTextures clusterTextures;
        TreeClusters treeClusters;
    };

    Clusters clusters;

    ModelWrapper trunkModel;
    Model* leafBilboardsModel{nullptr};

    Material leafMaterial;
    std::vector<LeafSSBO> allLeafs;
};
}  // namespace GameEngine
