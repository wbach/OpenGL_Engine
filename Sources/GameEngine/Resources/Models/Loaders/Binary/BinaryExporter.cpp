#include "BinaryExporter.h"

#include <GraphicsApi/MeshRawDataSerilizeHelpers.h>
#include <Logger/Log.h>
#include <Utils/Image/ImageSerializer.h>

#include <fstream>

#include "GameEngine/Resources/Textures/GeneralTexture.h"

namespace GameEngine
{
TextureSerilizeData convert(GeneralTexture* texture)
{
    if (texture)
    {
        TextureSerilizeData result;
        if (texture->GetFile())
        {
            result.path = texture->GetFile()->GetDataRelativePath().string();
        }
        else
        {
            result.image = texture->GetImage();
        }

        result.paramters = texture->getTextureParameters();
        return result;
    }

    return {};
}

MaterialSerilizeData convert(const Material& input)
{
    MaterialSerilizeData material;
    material.name        = input.name;
    material.ambient     = input.ambient.value;
    material.diffuse     = input.diffuse.value;
    material.specular    = input.specular.value;
    material.shineDamper = input.shineDamper;

    material.ambientTexture  = convert(input.ambientTexture);
    material.diffuseTexture  = convert(input.diffuseTexture);
    material.specularTexture = convert(input.specularTexture);

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

    material.baseColorTexture        = convert(input.baseColorTexture);
    material.normalTexture           = convert(input.normalTexture);
    material.metallicTexture         = convert(input.metallicTexture);
    material.roughnessTexture        = convert(input.roughnessTexture);
    material.ambientOcclusionTexture = convert(input.ambientOcclusionTexture);
    material.opacityTexture          = convert(input.opacityTexture);
    material.displacementTexture     = convert(input.displacementTexture);

    material.flags = input.flags;
    return material;
}

ModelSerializeData convert(const Model& model)
{
    ModelSerializeData modelSerializeData;
    modelSerializeData.normalizedFactor = model.getNormalizedFactor();
    modelSerializeData.skeleton_        = model.getRootJoint();

    for (const auto& mesh : model.GetMeshes())
    {
        modelSerializeData.meshes_.push_back({});
        auto& meshSerializedData = modelSerializeData.meshes_.back();

        meshSerializedData.meshRawData     = mesh.GetCMeshDataRef();
        meshSerializedData.material        = convert(mesh.GetMaterial());
        meshSerializedData.normalizedScale = mesh.GetNormalizedScale();
        meshSerializedData.renderType      = mesh.GetRenderType();
        meshSerializedData.transform       = mesh.GetMeshTransform();
    }

    return modelSerializeData;
}

void ExportModelBinary(const Model& model, const std::filesystem::path& path)
{
    auto outputPath = path;
    outputPath.replace_extension(".bin");

    std::ofstream file(outputPath, std::ios::binary);
    if (not file)
    {
        LOG_WARN << "Open file error: " << outputPath;
        return;
    }

    bitsery::Serializer<bitsery::OutputStreamAdapter> ser{file};
    ser.object(convert(model));

    if (not file.good())
    {
        LOG_ERROR << "File write error: " << outputPath;
        return;
    }

    LOG_DEBUG << "Export done: " << outputPath;
}
}  // namespace GameEngine
