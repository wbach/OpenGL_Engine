#include "BEngineLoader.h"
#include <Logger/Log.h>
#include <Utils/XML/XmlReader.h>
#include "GameEngine/Resources/ITextureLoader.h"
#include <algorithm>
#include <vector>

namespace GameEngine
{
namespace WBLoader
{
std::vector<float> convertFloatNode(const Utils::XmlNode& node)
{
    std::vector<float> result;
    auto v = Utils::SplitString(node.value_, ' ');

//    std::transform(v.back(), v.end(), std::back_inserter(result),
 //                  [](const std::string& input) -> float {
  //                     try
   //                    {
   //                        return std::stof(input);
    //                   }
     //                  catch (...)
      //                 {
       //                    return 0.f;
        //               }
         //          });

    return result;
}
std::vector<int32> convertIntNode(const Utils::XmlNode& node)
{
    return {};
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
    reader.Read(file.GetAbsoultePath());
    for (const auto& meshes : reader.Get()->GetChildren())
    {
        auto& mesh = model_->AddMesh(GraphicsApi::RenderType::TRIANGLES, graphicsApi_);

        const auto& verticesNode = meshes->GetChild("v");
        if (verticesNode)
            mesh.GetMeshDataRef().positions_ = convertFloatNode(*verticesNode);

        const auto& indicesNode = meshes->GetChild("i");
        if (verticesNode)
            mesh.GetMeshDataRef().indices_ = convertIntNode(*indicesNode);

        const auto& normalsNode = meshes->GetChild("vn");
        if (verticesNode)
            mesh.GetMeshDataRef().normals_ = convertFloatNode(*normalsNode);

        const auto& uvNode = meshes->GetChild("uv");
        if (verticesNode)
            mesh.GetMeshDataRef().textCoords_ = convertFloatNode(*uvNode);

        const auto& tanNode = meshes->GetChild("tan");
        if (verticesNode)
            mesh.GetMeshDataRef().tangents_ = convertFloatNode(*tanNode);

        const auto& btanNode = meshes->GetChild("btan");
        if (verticesNode)
            mesh.GetMeshDataRef().bitangents_ = convertFloatNode(*btanNode);

        const auto& joinIdsNode = meshes->GetChild("joinIds");
        if (verticesNode)
            mesh.GetMeshDataRef().joinIds_ = convertIntNode(*joinIdsNode);

        const auto& boneWeightsNode = meshes->GetChild("boneWeights");
        if (verticesNode)
            mesh.GetMeshDataRef().bonesWeights_ = convertFloatNode(*boneWeightsNode);
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
