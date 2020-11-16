#include "BEngineLoader.h"

#include <Logger/Log.h>
#include <Utils/XML/XmlReader.h>

#include <algorithm>
#include <vector>

#include "GameEngine/Resources/ITextureLoader.h"

namespace GameEngine
{
namespace WBLoader
{
std::vector<float> convertFloatNode(const Utils::XmlNode& node)
{
    std::vector<float> result;
    auto v = Utils::SplitString(node.value_, ' ');

    std::transform(v.begin(), v.end(), std::back_inserter(result), [](const std::string& input) -> float {
        try
        {
            return std::stof(input);
        }
        catch (...)
        {
            ERROR_LOG("convert error");
            return 0.f;
        }
    });

    return result;
}
std::vector<int32> convertIntNode(const Utils::XmlNode& node)
{
    std::vector<int32> result;
    auto v = Utils::SplitString(node.value_, ' ');

    std::transform(v.begin(), v.end(), std::back_inserter(result), [](const std::string& input) -> int32 {
        try
        {
            return std::stoi(input);
        }
        catch (...)
        {
            ERROR_LOG("convert error");
            return 0;
        }
    });

    return result;
}
BEngineLoader::BEngineLoader(ITextureLoader& textureLoader)
    : AbstractLoader(textureLoader.GetGraphicsApi(), textureLoader)
{
}

BEngineLoader::~BEngineLoader()
{
}

void BEngineLoader::ParseFile(const File& file)
{
    model_ = std::make_unique<Model>();
    Utils::XmlReader reader;
    if (not reader.Read(file.GetAbsoultePath()))
    {
        return;
    }

    for (const auto& meshes : reader.Get()->GetChildren())
    {
        auto& mesh = model_->AddMesh(GraphicsApi::RenderType::TRIANGLES, graphicsApi_);

        auto verticesNode = meshes->GetChild("v");
        if (verticesNode)
        {
            mesh.GetMeshDataRef().positions_ = convertFloatNode(*verticesNode);
        }
        else
        {
            ERROR_LOG("no vertices in object");
            continue;
        }
        auto positionSize = mesh.GetMeshDataRef().positions_.size();

        auto indicesNode = meshes->GetChild("i");
        if (indicesNode)
        {
            mesh.GetMeshDataRef().indices_ = convertIntNode(*indicesNode);
        }
        else
        {
            ERROR_LOG("no indices in in object");
            continue;
        }

        auto normalsNode = meshes->GetChild("vn");
        if (normalsNode)
        {
            mesh.GetMeshDataRef().normals_ = convertFloatNode(*normalsNode);
        }
        else
        {
            for (int x = 0; x < positionSize; x += 3)
            {
                mesh.GetMeshDataRef().normals_.push_back(0);
                mesh.GetMeshDataRef().normals_.push_back(1);
                mesh.GetMeshDataRef().normals_.push_back(0);
            }
        }

        auto uvNode = meshes->GetChild("uv");
        if (uvNode)
        {
            mesh.GetMeshDataRef().textCoords_ = convertFloatNode(*uvNode);
        }
        else
        {
            mesh.GetMeshDataRef().textCoords_ = std::vector<float>(positionSize, 0);
        }

        auto tanNode = meshes->GetChild("tan");
        if (tanNode)
        {
            mesh.GetMeshDataRef().tangents_ = convertFloatNode(*tanNode);
        }
        else
        {
            mesh.GetMeshDataRef().tangents_ = std::vector<float>(positionSize, 0);
        }

        auto btanNode = meshes->GetChild("btan");
        if (btanNode)
        {
            mesh.GetMeshDataRef().bitangents_ = convertFloatNode(*btanNode);
        }
        else
        {
            mesh.GetMeshDataRef().bitangents_ = std::vector<float>(positionSize, 0);
        }

        auto joinIdsNode = meshes->GetChild("joinIds");
        if (joinIdsNode)
        {
            mesh.GetMeshDataRef().joinIds_ = convertIntNode(*joinIdsNode);
        }
        else
        {
            mesh.GetMeshDataRef().joinIds_ = std::vector<int32>(positionSize, 0);
        }

        auto boneWeightsNode = meshes->GetChild("boneWeights");
        if (boneWeightsNode)
        {
            mesh.GetMeshDataRef().bonesWeights_ = convertFloatNode(*boneWeightsNode);
        }
        else
        {
            mesh.GetMeshDataRef().bonesWeights_ = std::vector<float>(positionSize, 0);
        }
    }
}

bool BEngineLoader::CheckExtension(const File& file)
{
    return file.IsExtension("bengine");
}

std::unique_ptr<Model> BEngineLoader::Create()
{
    return std::move(model_);
}

}  // namespace WBLoader
}  // namespace GameEngine
