#include "TreeGenerationParams.h"

#include <Utils/Json/JsonReader.h>
#include <Utils/Json/JsonWriter.h>

#include "Logger/Log.h"

namespace
{
static float ToFloat(const std::string& s, float def = 0.f)
{
    try
    {
        return std::stof(s);
    }
    catch (...)
    {
        return def;
    }
}

static int ToInt(const std::string& s, int def = 0)
{
    try
    {
        return std::stoi(s);
    }
    catch (...)
    {
        return def;
    }
}

static size_t ToSizeT(const std::string& s, size_t def = 0)
{
    try
    {
        return static_cast<size_t>(std::stoull(s));
    }
    catch (...)
    {
        return def;
    }
}

namespace TreeParams
{

constexpr char TreeGenerationParamsStr[] = "treeGenerationParams";
// SCALARS / ATTRIBUTES
constexpr char AttractorsCount[]         = "attractorsCount";
constexpr char TrunkMaterialTiledScale[] = "trunkMaterialTiledScale";
constexpr char MaxDistance[]             = "maxDistance";
constexpr char MinDistance[]             = "minDistance";
constexpr char SegmentLength[]           = "segmentLength";
constexpr char CrownYOffset[]            = "crownYOffset";

// TEXTURES
constexpr char TrunkBaseColor[]    = "trunkMaterialBaseColorTexture";
constexpr char TrunkAO[]           = "trunkMaterialAmbientOcclusionTexture";
constexpr char TrunkDisplacement[] = "trunkMaterialDisplacementTexture";
constexpr char TrunkMetallic[]     = "trunkMaterialMetallicTexture";
constexpr char TrunkNormal[]       = "trunkMaterialNormalTexture";
constexpr char TrunkRoughness[]    = "trunkMaterialRoughnessTexture";
constexpr char LeafBaseColor[]     = "leafMaterialBaseColorTexture";
constexpr char LeafOpacity[]       = "leafMaterialOpacityTexture";
constexpr char LeafRoughness[]     = "leafMaterialRoughnessTexture";
constexpr char LeafNormal[]        = "leafMaterialNormalTexture";

// VEC3 NODES
constexpr char CrownSize[]     = "crownSize";
constexpr char RootPosition[]  = "rootPosition";
constexpr char RootDirection[] = "rootDirection";

// MESH PARAMS NODE
constexpr char MeshBuilderNode[] = "meshBuilderParams";

// MESH PARAMS ATTRIBUTES
constexpr char RadialSegments[]          = "radialSegments";
constexpr char LeafHeightThreshold[]     = "leafheightTreshold";
constexpr char LeafRandomFactor[]        = "leafRandomFactor";
constexpr char LeafsPerBranch[]          = "leafsPerBranch";
constexpr char LeafSpread[]              = "leafSpread";
constexpr char MinBranchRadius[]         = "minBranchRadius";
constexpr char MaxBranchRadius[]         = "maxBranchRadius";
constexpr char TextureAtlasSize[]        = "textureAtlasSize";
constexpr char RadiusCreationThreshold[] = "radiusSizeCreationTreshold";
}  // namespace TreeParams

template <typename T>
T GetAttribute(const TreeNode& node, const std::string& key, T defaultValue)
{
    auto it = node.attributes_.find(key);
    if (it == node.attributes_.end())
    {
        return defaultValue;
    }

    if constexpr (std::is_same_v<T, float>)
    {
        return ToFloat(it->second, defaultValue);
    }
    else if constexpr (std::is_same_v<T, int>)
    {
        return ToInt(it->second, defaultValue);
    }
    else if constexpr (std::is_same_v<T, size_t>)
    {
        return ToSizeT(it->second, defaultValue);
    }
    else if constexpr (std::is_same_v<T, std::string>)
    {
        return it->second;
    }

    return defaultValue;
}

TreeNode BuildTree(const TreeGenerationParams& p)
{
    TreeNode root(TreeParams::TreeGenerationParamsStr);

    root.attributes_[TreeParams::AttractorsCount]         = std::to_string(p.attractorsCount);
    root.attributes_[TreeParams::TrunkMaterialTiledScale] = std::to_string(p.trunkMaterialTiledScale);
    root.attributes_[TreeParams::MaxDistance]             = std::to_string(p.maxDistance);
    root.attributes_[TreeParams::MinDistance]             = std::to_string(p.minDistance);
    root.attributes_[TreeParams::SegmentLength]           = std::to_string(p.segmentLength);
    root.attributes_[TreeParams::CrownYOffset]            = std::to_string(p.crownYOffset);

    root.attributes_[TreeParams::TrunkBaseColor]    = p.trunkMaterialBaseColorTexture;
    root.attributes_[TreeParams::TrunkAO]           = p.trunkMaterialAmbientOcclusionTexture;
    root.attributes_[TreeParams::TrunkDisplacement] = p.trunkMaterialDisplacementTexture;
    root.attributes_[TreeParams::TrunkMetallic]     = p.trunkMaterialMetallicTexture;
    root.attributes_[TreeParams::TrunkNormal]       = p.trunkMaterialNormalTexture;
    root.attributes_[TreeParams::TrunkRoughness]    = p.trunkMaterialRoughnessTexture;
    root.attributes_[TreeParams::LeafBaseColor]     = p.leafMaterialBaseColorTexture;
    root.attributes_[TreeParams::LeafOpacity]       = p.leafMaterialOpacityTexture;
    root.attributes_[TreeParams::LeafRoughness]     = p.leafMaterialRoughnessTexture;
    root.attributes_[TreeParams::LeafNormal]        = p.leafMaterialNormalTexture;

    auto writeVec3 = [&](const char* name, const vec3& v)
    {
        auto& node                = root.addChild(name);
        node.addChild("0").value_ = std::to_string(v.x);
        node.addChild("1").value_ = std::to_string(v.y);
        node.addChild("2").value_ = std::to_string(v.z);
    };

    writeVec3(TreeParams::CrownSize, p.crownSize);
    writeVec3(TreeParams::RootPosition, p.rootPosition);
    writeVec3(TreeParams::RootDirection, p.rootDirection);

    auto& mesh                                            = root.addChild(TreeParams::MeshBuilderNode);
    mesh.attributes_[TreeParams::RadialSegments]          = std::to_string(p.meshBuilderParams.radialSegments);
    mesh.attributes_[TreeParams::LeafHeightThreshold]     = std::to_string(p.meshBuilderParams.leafheightTreshold);
    mesh.attributes_[TreeParams::LeafRandomFactor]        = std::to_string(p.meshBuilderParams.leafRandomFactor);
    mesh.attributes_[TreeParams::LeafsPerBranch]          = std::to_string(p.meshBuilderParams.leafsPerBranch);
    mesh.attributes_[TreeParams::LeafSpread]              = std::to_string(p.meshBuilderParams.leafSpread);
    mesh.attributes_[TreeParams::MinBranchRadius]         = std::to_string(p.meshBuilderParams.minBranchRadius);
    mesh.attributes_[TreeParams::MaxBranchRadius]         = std::to_string(p.meshBuilderParams.maxBranchRadius);
    mesh.attributes_[TreeParams::TextureAtlasSize]        = std::to_string(p.meshBuilderParams.textureAtlasSize);
    mesh.attributes_[TreeParams::RadiusCreationThreshold] = std::to_string(p.meshBuilderParams.radiusSizeCreationTreshold);

    return root;
}

TreeGenerationParams LoadTreeParams(const TreeNode& root)
{
    TreeGenerationParams p;

    LOG_DEBUG << "node : " << root;

    p.attractorsCount         = GetAttribute(root, TreeParams::AttractorsCount, p.attractorsCount);
    p.trunkMaterialTiledScale = GetAttribute(root, TreeParams::TrunkMaterialTiledScale, p.trunkMaterialTiledScale);
    p.maxDistance             = GetAttribute(root, TreeParams::MaxDistance, p.maxDistance);
    p.minDistance             = GetAttribute(root, TreeParams::MinDistance, p.minDistance);
    p.segmentLength           = GetAttribute(root, TreeParams::SegmentLength, p.segmentLength);
    p.crownYOffset            = GetAttribute(root, TreeParams::CrownYOffset, p.crownYOffset);

    p.trunkMaterialBaseColorTexture        = GetAttribute(root, TreeParams::TrunkBaseColor, p.trunkMaterialBaseColorTexture);
    p.trunkMaterialAmbientOcclusionTexture = GetAttribute(root, TreeParams::TrunkAO, p.trunkMaterialAmbientOcclusionTexture);
    p.trunkMaterialDisplacementTexture = GetAttribute(root, TreeParams::TrunkDisplacement, p.trunkMaterialDisplacementTexture);
    p.trunkMaterialMetallicTexture     = GetAttribute(root, TreeParams::TrunkMetallic, p.trunkMaterialMetallicTexture);
    p.trunkMaterialNormalTexture       = GetAttribute(root, TreeParams::TrunkNormal, p.trunkMaterialNormalTexture);
    p.trunkMaterialRoughnessTexture    = GetAttribute(root, TreeParams::TrunkRoughness, p.trunkMaterialRoughnessTexture);
    p.leafMaterialBaseColorTexture     = GetAttribute(root, TreeParams::LeafBaseColor, p.leafMaterialBaseColorTexture);
    p.leafMaterialOpacityTexture       = GetAttribute(root, TreeParams::LeafOpacity, p.leafMaterialOpacityTexture);
    p.leafMaterialRoughnessTexture     = GetAttribute(root, TreeParams::LeafRoughness, p.leafMaterialRoughnessTexture);
    p.leafMaterialNormalTexture        = GetAttribute(root, TreeParams::LeafNormal, p.leafMaterialNormalTexture);

    if (auto* n = root.getChild(TreeParams::CrownSize))
        Read(*n, p.crownSize);
    if (auto* n = root.getChild(TreeParams::RootPosition))
        Read(*n, p.rootPosition);
    if (auto* n = root.getChild(TreeParams::RootDirection))
        Read(*n, p.rootDirection);

    if (auto* mNode = root.getChild(TreeParams::MeshBuilderNode))
    {
        p.meshBuilderParams.radialSegments = GetAttribute(*mNode, TreeParams::RadialSegments, p.meshBuilderParams.radialSegments);
        p.meshBuilderParams.leafheightTreshold =
            GetAttribute(*mNode, TreeParams::LeafHeightThreshold, p.meshBuilderParams.leafheightTreshold);
        p.meshBuilderParams.leafRandomFactor =
            GetAttribute(*mNode, TreeParams::LeafRandomFactor, p.meshBuilderParams.leafRandomFactor);
        p.meshBuilderParams.leafsPerBranch = GetAttribute(*mNode, TreeParams::LeafsPerBranch, p.meshBuilderParams.leafsPerBranch);
        p.meshBuilderParams.leafSpread     = GetAttribute(*mNode, TreeParams::LeafSpread, p.meshBuilderParams.leafSpread);
        p.meshBuilderParams.minBranchRadius =
            GetAttribute(*mNode, TreeParams::MinBranchRadius, p.meshBuilderParams.minBranchRadius);
        p.meshBuilderParams.maxBranchRadius =
            GetAttribute(*mNode, TreeParams::MaxBranchRadius, p.meshBuilderParams.maxBranchRadius);
        p.meshBuilderParams.textureAtlasSize =
            GetAttribute(*mNode, TreeParams::TextureAtlasSize, p.meshBuilderParams.textureAtlasSize);
        p.meshBuilderParams.radiusSizeCreationTreshold =
            GetAttribute(*mNode, TreeParams::RadiusCreationThreshold, p.meshBuilderParams.radiusSizeCreationTreshold);
    }

    return p;
}
}  // namespace

void SaveTreeGenerationParams(const TreeGenerationParams& params, const std::filesystem::path& path)
{
    auto root = BuildTree(params);
    Utils::Json::Write(path, root);
    LOG_DEBUG << "Done";
}
std::optional<TreeGenerationParams> ReadTreeGenerationParams(const std::filesystem::path& path)
{
    Utils::JsonReader reader;
    reader.Read(path);
    if (auto node = reader.Get(TreeParams::TreeGenerationParamsStr))
        return LoadTreeParams(*node);
    return {};
}
