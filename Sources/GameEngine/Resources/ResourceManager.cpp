#include "ResourceManager.h"

#include <algorithm>

#include <Logger/Log.h>
#include "GpuResourceLoader.h"
#include "TextureLoader.h"

namespace GameEngine
{
ResourceManager::ResourceManager(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuResourceLoader)
    : graphicsApi_(graphicsApi)
    , gpuResourceLoader_(gpuResourceLoader)
    , textureLoader_(std::make_unique<TextureLoader>(graphicsApi, gpuResourceLoader_, textures_))
    , loaderManager_(*textureLoader_)
{
}

ResourceManager::~ResourceManager()
{
    DEBUG_LOG("destructor");
}

Model* ResourceManager::LoadModel(const std::string& file)
{
    std::lock_guard<std::mutex> lk(modelMutex_);
    auto count = models_.count(file);

    if (count > 0)
    {
        auto& modelInfo = models_.at(file);
        ++modelInfo.instances_;
        DEBUG_LOG(file + " model already loaded, instances count : " + std::to_string(modelInfo.instances_));
        return modelInfo.resource_.get();
    }

    ResourceInfo<Model> modelInfo;
    modelInfo.resource_ = loaderManager_.Load(file);

    if (not modelInfo.resource_)
        return nullptr;

    auto modelPtr = modelInfo.resource_.get();
    models_.insert({file, std::move(modelInfo)});

    gpuResourceLoader_.AddObjectToGpuLoadingPass(*modelPtr);
    return modelPtr;
}

void ResourceManager::AddModel(std::unique_ptr<Model> model)
{
    auto modelPtr = model.get();

    ResourceInfo<Model> modelInfo;
    modelInfo.resource_ = std::move(model);

    auto filename = model->GetFileName().empty() ? ("UnknowFileModel_" + std::to_string(unknowFileNameResourceId_++))
                                                 : model->GetFileName();

    models_.insert({filename, std::move(modelInfo)});
    gpuResourceLoader_.AddObjectToGpuLoadingPass(*modelPtr);
}
void ResourceManager::ReleaseModel(Model& model)
{
    std::lock_guard<std::mutex> lk(modelMutex_);

    if (not models_.count(model.GetFileName()))
        return;

    auto& modelInfo = models_.at(model.GetFileName());
    --modelInfo.instances_;

    if (modelInfo.instances_ > 0)
        return;

    for (auto& mesh : modelInfo.resource_->GetMeshes())
    {
        DeleteMaterial(mesh.GetMaterial());
    }

    gpuResourceLoader_.AddObjectToRelease(std::move(modelInfo.resource_));
    models_.erase(model.GetFileName());
}

void ResourceManager::DeleteMaterial(const Material& material)
{
    if (material.diffuseTexture)
        textureLoader_->DeleteTexture(*material.diffuseTexture);
    if (material.ambientTexture)
        textureLoader_->DeleteTexture(*material.ambientTexture);
    if (material.normalTexture)
        textureLoader_->DeleteTexture(*material.normalTexture);
    if (material.specularTexture)
        textureLoader_->DeleteTexture(*material.specularTexture);
}
}  // namespace GameEngine
