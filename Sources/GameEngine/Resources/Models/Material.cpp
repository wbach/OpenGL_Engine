#include "Material.h"

#include <Logger/Log.h>
#include <Utils/Json/JsonReader.h>
#include <Utils/Json/JsonWriter.h>
#include <Utils/Utils.h>

#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/ITextureLoader.h"
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
        // -------------------- TEKSTURY --------------------
        auto loadTexture = [&](const char* key, GeneralTexture*& target)
        {
            if (auto node = reader.Get(key, root))
            {
                if (not node->value_.empty())
                {
                    TextureParameters parameters;
                    target = texLoader.LoadTexture(File(node->value_), parameters);
                }
            }
        };

        loadTexture("diffuseTexture", material.diffuseTexture);
        loadTexture("normalTexture", material.normalTexture);
        loadTexture("specularTexture", material.specularTexture);
        loadTexture("ambientTexture", material.ambientTexture);
        loadTexture("displacementTexture", material.displacementTexture);

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
                    auto x = std::stof(children[0]->value_);
                    auto y = std::stof(children[1]->value_);
                    auto z = std::stof(children[2]->value_);
                    auto w = std::stof(children[3]->value_);
                    target = Color(vec4(x, y, z, w));
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

        // -------------------- FLOATY --------------------
        if (auto node = reader.Get("shineDamper", root))
        {
            try
            {
                material.shineDamper = std::stof(node->value_);
            }
            catch (...)
            {
                LOG_ERROR << "Parse shineDamper error for node: " << (node ? node->type_ : "null");
            }
        }
        if (auto node = reader.Get("reflectivity", root))
        {
            try
            {
                material.reflectivity = std::stof(node->value_);
            }
            catch (...)
            {
                LOG_ERROR << "Parse reflectivity error for node: " << (node ? node->type_ : "null");
            }
        }
        if (auto node = reader.Get("indexOfRefraction", root))
        {
            try
            {
                material.indexOfRefraction = std::stof(node->value_);
            }
            catch (...)
            {
                LOG_ERROR << "Parse indexOfRefraction error for node: " << (node ? node->type_ : "null");
            }
        }

        // -------------------- BOOLE --------------------
        if (auto node = reader.Get("isTransparency", root))
        {
            material.isTransparency = Utils::StringToBool(node->value_);
        }
        if (auto node = reader.Get("useFakeLighting", root))
        {
            material.useFakeLighting = Utils::StringToBool(node->value_);
        }

        // -------------------- NAZWA --------------------
        if (auto node = reader.Get("name", root))
        {
            material.name = node->value_;
        }
    }
    else
    {
        LOG_ERROR << "Json root node not found in file: " << file;
    }

    return material;
}
void SaveMaterial(const Material& material, const File& requestedFile)
{
    auto file = requestedFile.CreateFileWithExtension("json");
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

    // -------------------- FLOATY --------------------
    root->addChild("shineDamper", std::to_string(material.shineDamper));
    root->addChild("reflectivity", std::to_string(material.reflectivity));
    root->addChild("indexOfRefraction", std::to_string(material.indexOfRefraction));

    // -------------------- BOOLE --------------------
    root->addChild("isTransparency", Utils::BoolToString(material.isTransparency));
    root->addChild("useFakeLighting", Utils::BoolToString(material.useFakeLighting));

    // -------------------- TEKSTURY --------------------
    if (auto node = textureToNode("diffuseTexture", material.diffuseTexture))
        root->addChild(std::move(node));
    else
        root->addChild("diffuseTexture");

    if (auto node = textureToNode("normalTexture", material.normalTexture))
        root->addChild(std::move(node));
    else
        root->addChild("normalTexture");
    if (auto node = textureToNode("specularTexture", material.specularTexture))
        root->addChild(std::move(node));
    else
        root->addChild("specularTexture");
    if (auto node = textureToNode("ambientTexture", material.ambientTexture))
        root->addChild(std::move(node));
    else
        root->addChild("ambientTexture");
    if (auto node = textureToNode("displacementTexture", material.displacementTexture))
        root->addChild(std::move(node));
    else
        root->addChild("displacementTexture");

    // -------------------- ZAPIS --------------------
    Utils::Json::Write(file.GetAbsolutePath().string(), *root);
}
}  // namespace GameEngine