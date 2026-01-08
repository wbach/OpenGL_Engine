#include "BinaryImporter.h"

#include <Logger/Log.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/bitsery.h>

#include <fstream>

#include "BinarySerializeHelpers.h"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/TextureParameters.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "GraphicsApi/IGraphicsApi.h"

namespace GameEngine
{
GeneralTexture* createTexture(ITextureLoader& textureLoader, const TextureSerilizeData& data)
{
    if (data.path.empty())
        return nullptr;

    LOG_DEBUG << "createTexture: " << data.path;
    return textureLoader.LoadTexture(data.path, data.paramters);
}
Material convert(ITextureLoader& textureLoader, const MaterialSerilizeData& input)
{
    LOG_DEBUG << "Convert material";

    Material material;
    material.name        = input.name;
    material.ambient     = input.ambient;
    material.diffuse     = input.diffuse;
    material.specular    = input.specular;
    material.shineDamper = input.shineDamper;

    material.ambientTexture  = createTexture(textureLoader, input.ambientTexture);
    material.diffuseTexture  = createTexture(textureLoader, input.diffuseTexture);
    material.specularTexture = createTexture(textureLoader, input.specularTexture);

    material.reflectivity      = input.reflectivity;
    material.indexOfRefraction = input.indexOfRefraction;
    material.isTransparency    = input.isTransparency;
    material.useFakeLighting   = input.useFakeLighting;

    material.baseColor        = input.baseColor;
    material.metallicFactor   = input.metallicFactor;
    material.roughnessFactor  = input.roughnessFactor;
    material.ambientOcclusion = input.ambientOcclusion;

    material.tiledScale         = input.tiledScale;
    material.normalScale        = input.normalScale;
    material.opacityCutoff      = input.opacityCutoff;
    material.subsurfaceStrength = input.subsurfaceStrength;
    material.subsurfaceColor    = input.subsurfaceColor;

    material.baseColorTexture        = createTexture(textureLoader, input.baseColorTexture);
    material.normalTexture           = createTexture(textureLoader, input.normalTexture);
    material.metallicTexture         = createTexture(textureLoader, input.metallicTexture);
    material.roughnessTexture        = createTexture(textureLoader, input.roughnessTexture);
    material.ambientOcclusionTexture = createTexture(textureLoader, input.ambientOcclusionTexture);
    material.opacityTexture          = createTexture(textureLoader, input.opacityTexture);
    material.displacementTexture     = createTexture(textureLoader, input.displacementTexture);

    return material;
}

std::unique_ptr<Model> ImportModelBinary(GraphicsApi::IGraphicsApi& graphicsApi, ITextureLoader& textureLoader,
                                         const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        LOG_ERROR << "Failed to open binary model file: " << path;
        return nullptr;
    }

    bitsery::Deserializer<bitsery::InputStreamAdapter> des{file};
    ModelSerializeData modelSerializeData;
    des.object(modelSerializeData);

    if (not file.good())
    {
        LOG_ERROR << "Binary model import failed: " << path;
        return nullptr;
    }

    auto model = std::make_unique<Model>();
    if (modelSerializeData.skeleton_)
    {
        model->setRootJoint(*modelSerializeData.skeleton_);
    }
    model->setNormailizedFactor(modelSerializeData.normalizedFactor);

    for (auto& meshSerilized : modelSerializeData.meshes_)
    {
        LOG_DEBUG << "Mesh positions size: " << meshSerilized.meshRawData.positions_.size();

        Mesh mesh(meshSerilized.renderType, graphicsApi, std::move(meshSerilized.meshRawData),
                  convert(textureLoader, meshSerilized.material), meshSerilized.transform, meshSerilized.normalizedScale);
        model->AddMesh(std::move(mesh));
    }
    return model;
}
}  // namespace GameEngine
