#include "AbstractLoader.h"

#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
#include "GameEngine/Resources/ResourceUtils.h"
#include "Logger/Log.h"
#include "Utils/Utils.h"

namespace GameEngine
{
AbstractLoader::AbstractLoader(GraphicsApi::IGraphicsApi& graphicsApi, ITextureLoader& textureLoader)
    : textureLoader_(textureLoader)
    , graphicsApi_(graphicsApi)
    , loadedFromBin_(false)
{
}
bool AbstractLoader::Parse(const File& file, const LoadingParameters& loadingParameters)
{
    loadingParameters_ = loadingParameters;
    fileName_          = file.GetFilename();
    filePath_          = file.GetAbsolutePath().parent_path().string();

    return ParseFile(file);
}
std::unique_ptr<Model> AbstractLoader::Create()
{
    auto startTime = std::chrono::high_resolution_clock::now();

    std::unique_ptr<Model> newModel = CreateModel();

    auto endTime  = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    LOG_DEBUG << "Model created. " << fileName_ << ". Time : " << duration << "ms. Meshes : " << newModel->GetMeshes().size();
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
        LOG_DEBUG << "Normalized boundingBox: " << boundingBox.min() << "-" << boundingBox.max();
    }

    auto newModel = std::make_unique<Model>(boundingBox);
    newModel->setNormailizedFactor(normalizeFactor);

    if (objects.empty())
    {
        LOG_ERROR << fileName_ << ". No object to create!";
    }
    for (auto& obj : objects)
    {
        if (obj.meshes.empty())
        {
            LOG_ERROR << fileName_ << ". No meshes in object!";
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
    LOG_DEBUG << "BoundingBox: " << modelBox.min() << "-" << modelBox.max();
    LOG_DEBUG << "Normalize factor : " << 1.f / modelBox.maxScale();
    return modelBox;
}
}  // namespace GameEngine
