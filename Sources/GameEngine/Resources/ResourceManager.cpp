#include "ResourceManager.h"
#include "GpuResourceLoader.h"
#include "Logger/Log.h"
#include "Models/Assimp/AssimpModel.h"
#include "TextureLoader.h"
namespace GameEngine
{
ResourceManager::ResourceManager(GraphicsApi::IGraphicsApi& graphicsApi)
    : graphicsApi_(graphicsApi)
    , gpuLoader_(std::make_shared<GpuResourceLoader>())
    , textureLoader_(std::make_shared<TextureLoader>(graphicsApi, textures_, gpuLoader_))
    , loaderManager_(*textureLoader_)
{
}

ResourceManager::~ResourceManager()
{
    Log(__FUNCTION__);
}

Model* ResourceManager::LoadModel(const std::string& file)
{
    auto count = modelsIds_.count(file);

    if (count > 0)
    {
        auto i = modelsIds_.at(file);
        Log("Model already loaded, id : " + std::to_string(modelsIds_.at(file)));
        return models_[i].get();
    }

    auto model = loaderManager_.Load(file);
    model->InitModel(file);
    modelsIds_.insert({ model->GetFileName(), models_.size() });
    models_.push_back(std::move(model));
    gpuLoader_->AddObjectToGpuLoadingPass(models_.back().get());
    return models_.back().get();
}

void ResourceManager::AddModel(Model* model)
{
    models_.emplace_back(model);
    modelsIds_.insert({ model->GetFileName() ,models_.size() - 1 });
    gpuLoader_->AddObjectToGpuLoadingPass(model);
}
}  // namespace GameEngine
