#include "TreeGenerationParams.h"

#include <Logger/Log.h>
#include <Utils/FileSystem/TextureSet.h>
#include <Utils/Json/JsonReader.h>
#include <Utils/Json/JsonWriter.h>

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

static vec2 ToVec2(const std::string& s, vec2 def = vec2(0.f))
{
    try
    {
        size_t commaPos = s.find(',');
        if (commaPos == std::string::npos)
        {
            return def;
        }

        float x = std::stof(s.substr(0, commaPos));
        float y = std::stof(s.substr(commaPos + 1));

        return vec2(x, y);
    }
    catch (...)
    {
        return def;
    }
}

static vec3 ToVec3(const std::string& s, vec3 def = vec3(0.f))
{
    try
    {
        size_t firstComma  = s.find(',');
        size_t secondComma = s.find(',', firstComma + 1);

        if (firstComma == std::string::npos || secondComma == std::string::npos)
            return def;

        float x = std::stof(s.substr(0, firstComma));
        float y = std::stof(s.substr(firstComma + 1, secondComma - firstComma - 1));
        float z = std::stof(s.substr(secondComma + 1));
        return vec3(x, y, z);
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
constexpr char LeafSize[]                = "leafSize";
constexpr char LeafTextureRotation[]     = "leafTexture";

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
    else if constexpr (std::is_same_v<T, vec2>)
    {
        return ToVec2(it->second, defaultValue);
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
    mesh.attributes_[TreeParams::LeafSize]                = std::to_string(p.meshBuilderParams.leafSize);
    mesh.attributes_[TreeParams::LeafTextureRotation]     = std::to_string(p.meshBuilderParams.leafTextureRotation);

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
        p.meshBuilderParams.leafSize = GetAttribute(*mNode, TreeParams::LeafSize, p.meshBuilderParams.leafSize);
        p.meshBuilderParams.leafTextureRotation =
            GetAttribute(*mNode, TreeParams::LeafTextureRotation, p.meshBuilderParams.leafTextureRotation);
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
void applyTextureSetToParams(TreeGenerationParams& params, const std::filesystem::path& stdPath)
{
    auto textures = Utils::findTexturesInDirectory(stdPath);

    LOG_DEBUG << "Zmapowano pliki z katalogu: " << stdPath;
    LOG_DEBUG << textures;

    std::string lowerPath = stdPath;
    std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);

    bool isTrunk = lowerPath.find("trunk") != std::string::npos || lowerPath.find("bark") != std::string::npos;
    bool isLeaf  = lowerPath.find("leaf") != std::string::npos || lowerPath.find("leafs") != std::string::npos;

    if (isTrunk)
    {
        LOG_DEBUG << "Rozpoznano zestaw tekstur: TRUNK";
        if (!textures.baseColor.empty())
            params.trunkMaterialBaseColorTexture = textures.baseColor.string();
        if (!textures.ambientOcclusion.empty())
            params.trunkMaterialAmbientOcclusionTexture = textures.ambientOcclusion.string();
        if (!textures.displacement.empty())
            params.trunkMaterialDisplacementTexture = textures.displacement.string();
        if (!textures.metallic.empty())
            params.trunkMaterialMetallicTexture = textures.metallic.string();
        if (!textures.normal.empty())
            params.trunkMaterialNormalTexture = textures.normal.string();
        if (!textures.roughness.empty())
            params.trunkMaterialRoughnessTexture = textures.roughness.string();
    }
    else if (isLeaf)
    {
        LOG_DEBUG << "Rozpoznano zestaw tekstur: LEAF";
        if (!textures.baseColor.empty())
            params.leafMaterialBaseColorTexture = textures.baseColor.string();
        if (!textures.opacity.empty())
            params.leafMaterialOpacityTexture = textures.opacity.string();
        if (!textures.roughness.empty())
            params.leafMaterialRoughnessTexture = textures.roughness.string();
        if (!textures.normal.empty())
            params.leafMaterialNormalTexture = textures.normal.string();
    }
    else
    {
        LOG_WARN << "Nie udało się jednoznacznie określić typu (Trunk/Leaf) na podstawie ścieżki: " << stdPath;
    }
}