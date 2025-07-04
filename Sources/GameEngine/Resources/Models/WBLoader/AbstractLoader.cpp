#include "AbstractLoader.h"

#include <fstream>

#include "Binary/BinaryReader.h"
#include "Binary/BinaryWriter.h"
#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/ITextureLoader.h"
#include "Logger/Log.h"
#include "Resources/Models/BoundingBox.h"
#include "Utils/Utils.h"

namespace GameEngine
{
namespace WBLoader
{
AbstractLoader::AbstractLoader(GraphicsApi::IGraphicsApi& graphicsApi, ITextureLoader& textureLoader)
    : textureLoader_(textureLoader)
    , graphicsApi_(graphicsApi)
    , loadedFromBin_(false)
{
}
void AbstractLoader::Parse(const File& file, const LoadingParameters& loadingParameters)
{
    loadingParameters_ = loadingParameters;
    fileName_ = file.GetFilename();
    filePath_ = file.GetParentDir();

    auto binFile = CreateBinPath(fileName_);
    if (EngineConf.useBinaryLoading && Utils::CheckFileExist(binFile))
    {
        ReadBinFile(binFile, textureLoader_);
        loadedFromBin_ = true;
        fileName_      = binFile;
        DEBUG_LOG("Load from bin file :" + fileName_);
    }
    else
    {
        ParseFile(file);
    }
}
std::unique_ptr<Model> AbstractLoader::Create()
{
    auto startTime = std::chrono::high_resolution_clock::now();

    std::unique_ptr<Model> newModel = CreateModel();

    //   if (loadedFromBin_)
    //   {
    //       newModel       = CreateModelFromBin();
    //       loadedFromBin_ = false;
    //   }
    //   else
    //   {
    //       newModel = CreateModel();
    //       CreateBinFile(newModel, fileName_);
    //   }

    auto endTime  = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    DEBUG_LOG("Model created. " + fileName_ + ". Time : " + std::to_string(duration) +
              "ms. Meshes : " + std::to_string(newModel->GetMeshes().size()));
    return newModel;
}
std::unique_ptr<Model> AbstractLoader::CreateModel()
{
    auto boundingBox = getModelBoundingBox();

    float normalizeFactor{1.f};
    if (loadingParameters_.modelNormalization == ModelNormalization::normalized)
    {
        normalizeFactor = 1.f / boundingBox.maxScale();
        boundingBox.scale(vec3(normalizeFactor));
        DEBUG_LOG("Normalized boundingBox: " + std::to_string(boundingBox.min()) + "-" + std::to_string(boundingBox.max())  );
    }

    auto newModel = std::make_unique<Model>(boundingBox);
    newModel->setNormailizedFactor(normalizeFactor);

    if (objects.empty())
    {
        ERROR_LOG(fileName_ + ". No object to create!");
    }
    for (auto& obj : objects)
    {
        if (obj.meshes.empty())
        {
            ERROR_LOG(fileName_ + ". No meshes in object!");
        }

        NormalizeMatrix(obj.transformMatrix, normalizeFactor);

        Animation::CalcInverseBindTransform(obj.skeleton_);
        newModel->setRootJoint(std::move(obj.skeleton_));
        newModel->animationClips_ = std::move(obj.animationClips_);

        for (auto& mesh : obj.meshes)
        {
            newModel->AddMesh(GraphicsApi::RenderType::TRIANGLES, graphicsApi_, mesh.createMeshRawData(), mesh.material,
                              obj.transformMatrix, vec3(normalizeFactor));
        }
    }
    objects.clear();
    return newModel;
}

std::unique_ptr<Model> AbstractLoader::CreateModelFromBin()
{
    return ReadBinFile(fileName_, textureLoader_);
}
void AbstractLoader::NormalizeMatrix(mat4& mat, float factor) const
{
    mat *= glm::scale(vec3(factor));
}
BoundingBox AbstractLoader::getModelBoundingBox() const
{
    vec3 min = vec3(std::numeric_limits<float>::max());
    vec3 max = vec3(-std::numeric_limits<float>::max());

    for (auto& obj : objects)
    {
        for (auto& mesh : obj.meshes)
        {
            auto meshBox = mesh.getBoundingBox();

            if (min.x > meshBox.min().x)
                min.x = meshBox.min().x;
            if (min.y > meshBox.min().y)
                min.y = meshBox.min().y;
            if (min.z > meshBox.min().z)
                min.z = meshBox.min().z;

            if (max.x < meshBox.max().x)
                max.x = meshBox.max().x;
            if (max.y < meshBox.max().y)
                max.y = meshBox.max().y;
            if (max.z < meshBox.max().z)
                max.z = meshBox.max().z;
        }
    }

    BoundingBox modelBox;
    modelBox.minMax(min, max);
    DEBUG_LOG("BoundingBox: " + std::to_string(modelBox.min()) + "-" + std::to_string(modelBox.max())  );
    DEBUG_LOG("Normalize factor : " + std::to_string(1.f / modelBox.maxScale()));
    return modelBox;
}
}  // namespace WBLoader
}  // namespace GameEngine
