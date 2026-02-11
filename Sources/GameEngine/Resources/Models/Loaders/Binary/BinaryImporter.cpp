#include "BinaryImporter.h"

#include <Logger/Log.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/bitsery.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <fstream>

#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/TextureParameters.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "Image/Image.h"

namespace GameEngine
{
GeneralTexture* createTexture(ITextureLoader& textureLoader, const TextureSerilizeData& data)
{
    if (not data.path.empty())
    {
        LOG_DEBUG << "createTexture: " << data.path;
        return textureLoader.LoadTexture(data.path, data.paramters);
    }

    if (data.image.has_value())
    {
        LOG_DEBUG << "Binary Has Texture without file IAMGE: " << data.image.value();
        Utils::Image copyImage = data.image.value();
        if (copyImage.isCompressed)
        {
            copyImage.decompressData();
        }
        return textureLoader.CreateTexture(Utils::CreateUniqueFilename(), data.paramters, std::move(copyImage));
    }

    return nullptr;
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

    material.flags = input.flags;

    return material;
}

std::unique_ptr<Model> convert(ModelSerializeData&& modelSerializeData, GraphicsApi::IGraphicsApi& graphicsApi,
                               ITextureLoader& textureLoader)
{
    if (modelSerializeData.meshes_.empty())
    {
        LOG_DEBUG << "No mesh in model";
        return nullptr;
    }
    auto model = std::make_unique<Model>();
    if (modelSerializeData.skeleton_)
    {
        model->setRootJoint(std::move(*modelSerializeData.skeleton_));
        modelSerializeData.skeleton_.reset();
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
    return convert(std::move(modelSerializeData), graphicsApi, textureLoader);
}
}  // namespace GameEngine
