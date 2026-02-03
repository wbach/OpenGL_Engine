#pragma once
#include <GameEngine/Components/Renderer/Trees/TreeMeshBuilder.h>
#include <Types.h>
#include <Utils/TreeNode.h>

#include <filesystem>

struct TreeGenerationParams
{
    size_t attractorsCount        = 400;
    vec3 crownSize                = vec3{10.f, 6.f, 8.f};
    float trunkMaterialTiledScale = 2.f;
    float maxDistance{5.f};
    float minDistance{1.f};
    float segmentLength{0.3f};
    float crownYOffset{3.f};
    vec3 rootPosition{0.f};
    vec3 rootDirection{0.f, 1.f, 0.f};
    std::string trunkMaterialBaseColorTexture        = "Textures/Tree/trunk/light-tree-bark_albedo.png";
    std::string trunkMaterialAmbientOcclusionTexture = "Textures/Tree/trunk/light-tree-bark_ao.png";
    std::string trunkMaterialDisplacementTexture     = "Textures/Tree/trunk/light-tree-bark_albedo.png";
    std::string trunkMaterialMetallicTexture         = "Textures/Tree/trunk/light-tree-bark_metallic.png";
    std::string trunkMaterialNormalTexture           = "Textures/Tree/trunk/light-tree-bark_normal-ogl.png";
    std::string trunkMaterialRoughnessTexture        = "Textures/Tree/trunk/light-tree-bark_roughness.png";
    std::string leafMaterialBaseColorTexture         = "Textures/Tree/Leafs/LeafSet024_2K-PNG_ColorWB.png";
    std::string leafMaterialOpacityTexture           = "Textures/Tree/Leafs/LeafSet024_2K-PNG_Opacity.png";
    std::string leafMaterialRoughnessTexture         = "Textures/Tree/Leafs/LeafSet024_2K-PNG_Roughness.png";
    std::string leafMaterialNormalTexture            = "Textures/Tree/Leafs/LeafSet024_2K-PNG_NormalGL.png";

    GameEngine::TreeMeshBuilder::EntryParameters meshBuilderParams;
};

void SaveTreeGenerationParams(const TreeGenerationParams&, const std::filesystem::path&);
std::optional<TreeGenerationParams> ReadTreeGenerationParams(const std::filesystem::path&);
