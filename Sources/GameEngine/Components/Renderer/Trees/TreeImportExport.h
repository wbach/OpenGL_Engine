#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include <Types.h>

#include <filesystem>
#include <vector>

#include "Image/Image.h"

namespace GameEngine
{
class ITextureLoader;
struct Material;

struct Leaf
{
    vec3 position;
    vec3 direction;
    int textureIndex;
    vec3 colorRandomness;
    float sizeRandomness;
};

struct Cluster
{
    vec3 minBound;
    vec3 maxBound;
    std::vector<uint32> leafIndices;
};

struct TreeClusters
{
    vec3 gridOrigin;
    vec3 voxelSize;
    vec3i gridSize;
    std::vector<Cluster> clusters;
};

struct Billboard
{
    int resolution;
    std::vector<float> alpha;

    vec3 center;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;

    float extentU;
    float extentV;
};

struct ClusterTextures
{
    GraphicsApi::ID baseColorTextureArray;
    GraphicsApi::ID normalTextureArray;

    std::vector<Utils::Image> baseColorImages;
    std::vector<Utils::Image> normalImages;
};

struct LeafSSBO
{
    AlignWrapper<vec4> positionAndSizeRandomness;
    AlignWrapper<vec4> direction;  // w not used
    AlignWrapper<vec4> colorRandomnessAndTextureIndex;
};


struct ClustersData
{
    TreeClusters clusters;
    std::vector<Utils::Image> baseColorTextures;
    std::vector<Utils::Image> normalsTextures;
};

struct LeafsData
{
    std::vector<LeafSSBO> leafsSsbo;
    Material material;
    ClustersData clusters;

};

struct TrunkData
{
    ModelSerializeData modelData;
    ModelSerializeData lodData;
    Material material;
};

struct TreeData
{
    LeafsData leafsData;
    TrunkData trunkData;
};


void ExportTree
}  // namespace GameEngine
