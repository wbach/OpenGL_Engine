#include "Material.h"

#include <Logger/Log.h>
#include <Utils/Json/JsonReader.h>
#include <Utils/Json/JsonWriter.h>
#include <Utils/Utils.h>

#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/TextureParameters.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "Types.h"

namespace GameEngine
{
Material ParseMaterial(const File& file, ITextureLoader& texLoader)
{
    Material material;
    Utils::JsonReader reader;
    reader.Read(file.GetAbsolutePath().string());

    if (auto root = reader.Get())
    {
        auto loadTexture = [&](const char* key, const TextureParameters& textParams = {}) -> GeneralTexture*
        {
            if (auto node = reader.Get(key, root))
            {
                if (!node->value_.empty())
                {
                    File texFile(node->value_);
                    if (texFile.exist())
                        return texLoader.LoadTexture(texFile, textParams);

                    LOG_WARN << "Load texture error! File not found: " << node->value_;
                }
            }
            return nullptr;
        };

        TextureParameters textureParams;
        textureParams.mimap = GraphicsApi::TextureMipmap::LINEAR;

        // --- PBR Textures ---
        material.baseColorTexture    = loadTexture("baseColorTexture", textureParams);
        material.normalTexture       = loadTexture("normalTexture", textureParams);
        material.metallicTexture     = loadTexture("metallicTexture", textureParams);
        material.roughnessTexture    = loadTexture("roughnessTexture", textureParams);
        material.occlusionTexture    = loadTexture("occlusionTexture", textureParams);
        material.emissionTexture     = loadTexture("emissionTexture", textureParams);
        material.opacityTexture      = loadTexture("opacityTexture", textureParams);
        material.displacementTexture = loadTexture("displacementTexture", textureParams);

        // --- PBR Parameters (Vectors) ---
        auto parseVec4 = [&](const char* key, vec4& target)
        {
            if (auto node = reader.Get(key, root))
            {
                const auto& children = node->getChildren();
                if (children.size() >= 3)
                {
                    target.x = std::stof(children[0]->value_);
                    target.y = std::stof(children[1]->value_);
                    target.z = std::stof(children[2]->value_);
                    if (children.size() > 3)
                        target.w = std::stof(children[3]->value_);
                }
            }
        };

        auto parseVec3 = [&](const char* key, vec3& target)
        {
            if (auto node = reader.Get(key, root))
            {
                const auto& children = node->getChildren();
                if (children.size() >= 3)
                {
                    target.x = std::stof(children[0]->value_);
                    target.y = std::stof(children[1]->value_);
                    target.z = std::stof(children[2]->value_);
                }
            }
        };

        parseVec4("baseColor", material.baseColor.value);
        parseVec3("subsurfaceColor", material.subsurfaceColor);

        // --- PBR Parameters (Floats) ---
        auto parseFloat = [&](const char* key, float& target)
        {
            if (auto node = reader.Get(key, root))
            {
                try
                {
                    target = std::stof(node->value_);
                }
                catch (...)
                {
                    LOG_ERROR << "Parse float error for key: " << key;
                }
            }
        };

        parseFloat("metallicFactor", material.metallicFactor);
        parseFloat("roughnessFactor", material.roughnessFactor);
        parseFloat("normalScale", material.normalScale);
        parseFloat("occlusionStrength", material.occlusionStrength);
        parseFloat("indexOfRefraction", material.indexOfRefraction);
        parseFloat("transmission", material.transmission);
        parseFloat("opacityCutoff", material.opacityCutoff);
        parseFloat("subsurfaceStrength", material.subsurfaceStrength);
        parseFloat("uvScale", material.uvScale);

        // --- Metadata & Flags ---
        if (auto node = reader.Get("name", root))
            material.name = node->value_;

        if (auto node = reader.Get("flags", root))
            material.flags = static_cast<uint32_t>(std::stoul(node->value_));
    }
    else
    {
        LOG_ERROR << "Json root node not found in file: " << file;
    }

    return material;
}

void SaveMaterial(const Material& material, const File& requestedFile)
{
    auto file = requestedFile.HasExtension() ? requestedFile : requestedFile.CreateFileWithExtension("material");
    auto root = std::make_unique<TreeNode>("Material");

    auto vec4ToNode = [](const std::string& name, const vec4& v)
    {
        auto node = std::make_unique<TreeNode>(name);
        node->addChild("0", std::to_string(v.x));
        node->addChild("1", std::to_string(v.y));
        node->addChild("2", std::to_string(v.z));
        node->addChild("3", std::to_string(v.w));
        return node;
    };

    auto vec3ToNode = [](const std::string& name, const vec3& v)
    {
        auto node = std::make_unique<TreeNode>(name);
        node->addChild("0", std::to_string(v.x));
        node->addChild("1", std::to_string(v.y));
        node->addChild("2", std::to_string(v.z));
        return node;
    };

    auto texToNode = [](const std::string& name, GeneralTexture* tex)
    {
        if (!tex || !tex->GetFile())
            return std::make_unique<TreeNode>(name, "");
        return std::make_unique<TreeNode>(name, tex->GetFile()->GetDataRelativePath());
    };

    // Serializacja danych
    root->addChild("name", material.name);
    root->addChild("flags", std::to_string(material.flags));

    root->addChild(vec4ToNode("baseColor", material.baseColor.value));
    root->addChild(vec3ToNode("subsurfaceColor", material.subsurfaceColor));

    root->addChild("metallicFactor", std::to_string(material.metallicFactor));
    root->addChild("roughnessFactor", std::to_string(material.roughnessFactor));
    root->addChild("normalScale", std::to_string(material.normalScale));
    root->addChild("occlusionStrength", std::to_string(material.occlusionStrength));
    root->addChild("indexOfRefraction", std::to_string(material.indexOfRefraction));
    root->addChild("transmission", std::to_string(material.transmission));
    root->addChild("opacityCutoff", std::to_string(material.opacityCutoff));
    root->addChild("subsurfaceStrength", std::to_string(material.subsurfaceStrength));
    root->addChild("uvScale", std::to_string(material.uvScale));

    root->addChild(texToNode("baseColorTexture", material.baseColorTexture));
    root->addChild(texToNode("normalTexture", material.normalTexture));
    root->addChild(texToNode("metallicTexture", material.metallicTexture));
    root->addChild(texToNode("roughnessTexture", material.roughnessTexture));
    root->addChild(texToNode("occlusionTexture", material.occlusionTexture));
    root->addChild(texToNode("emissionTexture", material.emissionTexture));
    root->addChild(texToNode("opacityTexture", material.opacityTexture));
    root->addChild(texToNode("displacementTexture", material.displacementTexture));

    Utils::Json::Write(file.GetAbsolutePath().string(), *root);
}
}  // namespace GameEngine
