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

    LOG_DEBUG << "reader: " << file.GetAbsolutePath();
    Utils::JsonReader reader;
    reader.Read(file.GetAbsolutePath().string());

    if (auto root = reader.Get())
    {
        auto loadTexture = [&](const char* key, const TextureParameters& textParams = {}) -> GeneralTexture*
        {
            if (auto node = reader.Get(key, root))
            {
                LOG_DEBUG << node->name();
                if (!node->value_.empty())
                {
                    File file(node->value_);
                    if (file.exist())
                    {
                        return texLoader.LoadTexture(File(node->value_), textParams);
                    }
                    else
                    {
                        LOG_WARN << "Load texture error!  " << node->value_;
                    }
                }
            }
            return nullptr;
        };

        TextureParameters textureParams;
        textureParams.mimap = GraphicsApi::TextureMipmap::LINEAR;

        // --- legacy Phong textures ---
        material.diffuseTexture      = loadTexture("diffuseTexture", textureParams);
        material.normalTexture       = loadTexture("normalTexture", textureParams);
        material.specularTexture     = loadTexture("specularTexture", textureParams);
        material.ambientTexture      = loadTexture("ambientTexture", textureParams);
        material.displacementTexture = loadTexture("displacementTexture", textureParams);

        // --- PBR textures ---
        material.baseColorTexture        = loadTexture("baseColorTexture", textureParams);
        material.metallicTexture         = loadTexture("metallicTexture", textureParams);
        material.roughnessTexture        = loadTexture("roughnessTexture", textureParams);
        material.ambientOcclusionTexture = loadTexture("ambientOcclusionTexture", textureParams);
        material.opacityTexture          = loadTexture("opacityTexture", textureParams);

        // -------------------- KOLORY --------------------
        auto parseColor = [&](const TreeNode* node, Color& target)
        {
            try
            {
                if (!node)
                    return;
                const auto& children = node->getChildren();
                if (children.size() >= 3)
                {
                    auto x  = std::stof(children[0]->value_);
                    auto y  = std::stof(children[1]->value_);
                    auto z  = std::stof(children[2]->value_);
                    float w = (children.size() > 3) ? std::stof(children[3]->value_) : 1.f;
                    target  = Color(vec4(x, y, z, w));
                }
            }
            catch (...)
            {
                LOG_ERROR << "Parse vector error for node: " << (node ? node->type_ : "null");
            }
        };

        if (auto node = reader.Get("diffuseColor", root))
            parseColor(node, material.diffuse);
        if (auto node = reader.Get("ambientColor", root))
            parseColor(node, material.ambient);
        if (auto node = reader.Get("specularColor", root))
            parseColor(node, material.specular);
        if (auto node = reader.Get("baseColor", root))
        {
            const auto& children = node->getChildren();
            if (children.size() >= 3)
            {
                auto x             = std::stof(children[0]->value_);
                auto y             = std::stof(children[1]->value_);
                auto z             = std::stof(children[2]->value_);
                float w            = (children.size() > 3) ? std::stof(children[3]->value_) : 1.f;
                material.baseColor = vec4(x, y, z, w);
            }
        }

        // -------------------- FLOATY --------------------
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
                    LOG_ERROR << "Parse float error for node: " << (node ? node->type_ : "null");
                }
            }
        };

        parseFloat("shineDamper", material.shineDamper);
        parseFloat("reflectivity", material.reflectivity);
        parseFloat("indexOfRefraction", material.indexOfRefraction);
        parseFloat("metallicFactor", material.metallicFactor);
        parseFloat("roughnessFactor", material.roughnessFactor);
        parseFloat("ambientOcclusion", material.ambientOcclusion);
        parseFloat("tiledScale", material.tiledScale);
        parseFloat("normalScale", material.normalScale);
        parseFloat("opacityCutoff", material.opacityCutoff);
        parseFloat("subsurfaceStrength", material.subsurfaceStrength);

        if (auto node = reader.Get("subsurfaceColor", root))
        {
            const auto& children = node->getChildren();
            if (children.size() >= 3)
            {
                float x                  = std::stof(children[0]->value_);
                float y                  = std::stof(children[1]->value_);
                float z                  = std::stof(children[2]->value_);
                material.subsurfaceColor = vec3(x, y, z);
            }
        }

        // -------------------- BOOLE --------------------
        auto parseBool = [&](const char* key, bool& target)
        {
            if (auto node = reader.Get(key, root))
                target = Utils::StringToBool(node->value_);
        };

        parseBool("isTransparency", material.isTransparency);
        parseBool("useFakeLighting", material.useFakeLighting);

        // -------------------- NAZWA --------------------
        if (auto node = reader.Get("name", root))
            material.name = node->value_;
    }
    else
    {
        LOG_ERROR << "Json root node not found in file: " << file;
    }

    LOG_DEBUG << "return material";
    return material;
}

void SaveMaterial(const Material& material, const File& requestedFile)
{
    auto file = requestedFile.HasExtension() ? requestedFile : requestedFile.CreateFileWithExtension("material");
    auto root = std::make_unique<TreeNode>("Material");

    auto colorToNode = [](const std::string& name, const Color& v) -> std::unique_ptr<TreeNode>
    {
        auto node = std::make_unique<TreeNode>(name);
        node->addChild("0", std::to_string(v.value.x));
        node->addChild("1", std::to_string(v.value.y));
        node->addChild("2", std::to_string(v.value.z));
        node->addChild("3", std::to_string(v.value.w));
        return node;
    };

    auto vec4ToNode = [](const std::string& name, const glm::vec4& v) -> std::unique_ptr<TreeNode>
    {
        auto node = std::make_unique<TreeNode>(name);
        node->addChild("0", std::to_string(v.x));
        node->addChild("1", std::to_string(v.y));
        node->addChild("2", std::to_string(v.z));
        node->addChild("3", std::to_string(v.w));
        return node;
    };

    auto vec3ToNode = [](const std::string& name, const glm::vec3& v) -> std::unique_ptr<TreeNode>
    {
        auto node = std::make_unique<TreeNode>(name);
        node->addChild("0", std::to_string(v.x));
        node->addChild("1", std::to_string(v.y));
        node->addChild("2", std::to_string(v.z));
        return node;
    };

    auto textureToNode = [](const std::string& name, GeneralTexture* tex) -> std::unique_ptr<TreeNode>
    {
        if (!tex)
            return nullptr;
        return std::make_unique<TreeNode>(name, tex->GetFile()->GetDataRelativePath().string());
    };

    // -------------------- NAZWA --------------------
    root->addChild("name", material.name);

    // -------------------- KOLORY --------------------
    root->addChild(colorToNode("diffuseColor", material.diffuse));
    root->addChild(colorToNode("ambientColor", material.ambient));
    root->addChild(colorToNode("specularColor", material.specular));
    root->addChild(vec4ToNode("baseColor", material.baseColor));
    root->addChild(vec3ToNode("subsurfaceColor", material.subsurfaceColor));

    // -------------------- FLOATY --------------------
    root->addChild("shineDamper", std::to_string(material.shineDamper));
    root->addChild("reflectivity", std::to_string(material.reflectivity));
    root->addChild("indexOfRefraction", std::to_string(material.indexOfRefraction));
    root->addChild("metallicFactor", std::to_string(material.metallicFactor));
    root->addChild("roughnessFactor", std::to_string(material.roughnessFactor));
    root->addChild("ambientOcclusion", std::to_string(material.ambientOcclusion));
    root->addChild("tiledScale", std::to_string(material.tiledScale));
    root->addChild("normalScale", std::to_string(material.normalScale));
    root->addChild("opacityCutoff", std::to_string(material.opacityCutoff));
    root->addChild("subsurfaceStrength", std::to_string(material.subsurfaceStrength));

    // -------------------- BOOLE --------------------
    root->addChild("isTransparency", Utils::BoolToString(material.isTransparency));
    root->addChild("useFakeLighting", Utils::BoolToString(material.useFakeLighting));

    // -------------------- TEKSTURY --------------------
    auto addTextureNode = [&](const char* name, GeneralTexture* tex)
    {
        if (auto node = textureToNode(name, tex))
            root->addChild(std::move(node));
        else
            root->addChild(name);
    };

    addTextureNode("diffuseTexture", material.diffuseTexture);
    addTextureNode("ambientTexture", material.ambientTexture);
    addTextureNode("specularTexture", material.specularTexture);
    addTextureNode("displacementTexture", material.displacementTexture);

    addTextureNode("baseColorTexture", material.baseColorTexture);
    addTextureNode("normalTexture", material.normalTexture);
    addTextureNode("metallicTexture", material.metallicTexture);
    addTextureNode("roughnessTexture", material.roughnessTexture);
    addTextureNode("ambientOcclusionTexture", material.ambientOcclusionTexture);
    addTextureNode("opacityTexture", material.opacityTexture);

    // -------------------- ZAPIS --------------------
    Utils::Json::Write(file.GetAbsolutePath().string(), *root);
}
}  // namespace GameEngine
