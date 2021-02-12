#include "ResourceManager.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GpuResourceLoader.h"
#include "TextureLoader.h"

namespace GameEngine
{
ResourceManager::ResourceManager(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuResourceLoader)
    : graphicsApi_(graphicsApi)
    , gpuResourceLoader_(gpuResourceLoader)
    , textureLoader_(std::make_unique<TextureLoader>(graphicsApi, gpuResourceLoader_))
    , loaderManager_(*textureLoader_)
    , unknowFileNameResourceId_(0)
    , releaseLockState_(true)
{
}

ResourceManager::~ResourceManager()
{
    releaseLockState_ = true;
    std::vector<Model*> toRelease;
    for (auto& model : models_)
        toRelease.push_back(model.second.resource_.get());
    DEBUG_LOG("Release not deleted models. size :" + std::to_string(toRelease.size()));
    for (auto model : toRelease)
        ReleaseModel(*model);

    textureLoader_.reset();
}

Model* ResourceManager::LoadModel(const File& file)
{
    auto absoultePath = file.GetAbsoultePath();
    std::lock_guard<std::mutex> lk(modelMutex_);
    auto count = models_.count(absoultePath);

    if (count > 0)
    {
        auto& modelInfo = models_.at(absoultePath);
        ++modelInfo.instances_;
        modelInfo.resourceGpuStatus_ = ResourceGpuStatus::Loaded;
        // ResourceGpuStatus::NotLoaded for models not implmented. T
        // To do: Can be useful for simplified physics collision mesh shapes.
        // Visual representation of physics shape not needed
        DEBUG_LOG(file.GetBaseName() +
                  " model already loaded, instances count : " + std::to_string(modelInfo.instances_));
        return modelInfo.resource_.get();
    }

    ResourceInfo<Model> modelInfo;
    modelInfo.resource_ = loaderManager_.Load(file);

    if (not modelInfo.resource_)
        return nullptr;

    auto modelPtr = modelInfo.resource_.get();
    models_.insert({absoultePath, std::move(modelInfo)});

    gpuResourceLoader_.AddObjectToGpuLoadingPass(*modelPtr);
    return modelPtr;
}

void ResourceManager::AddModel(std::unique_ptr<Model> model)
{
    DEBUG_LOG("add model.");
    auto modelPtr = model.get();
    auto filename = model->GetFile() ? ("UnknowFileModel_" + std::to_string(unknowFileNameResourceId_++))
                                     : model->GetFile().GetAbsoultePath();

    ResourceInfo<Model> modelInfo;
    modelInfo.resource_ = std::move(model);

    models_.insert({filename, std::move(modelInfo)});
    gpuResourceLoader_.AddObjectToGpuLoadingPass(*modelPtr);
}
void ResourceManager::ReleaseModel(Model& model)
{
    std::lock_guard<std::mutex> lk(modelMutex_);

    auto absoultePath = model.GetFile().GetAbsoultePath();

    if (not models_.count(absoultePath))
        return;

    auto& modelInfo = models_.at(absoultePath);
    --modelInfo.instances_;

    if (modelInfo.instances_ > 0 and releaseLockState_)
        return;

    for (auto& mesh : modelInfo.resource_->GetMeshes())
    {
        DeleteMaterial(mesh.GetMaterial());
    }

    gpuResourceLoader_.AddObjectToRelease(std::move(modelInfo.resource_));
    models_.erase(absoultePath);
    DEBUG_LOG("models_ erase , size : " + std::to_string(models_.size()));
}

void ResourceManager::LockReleaseResources()
{
    releaseLockState_ = false;
    textureLoader_->LockReleaseResources();
}

void ResourceManager::UnlockReleaseResources()
{
    releaseLockState_ = true;
    textureLoader_->UnlockReleaseResources();
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
