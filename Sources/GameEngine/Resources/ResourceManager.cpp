#include "ResourceManager.h"

#include <algorithm>

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
    DEBUG_LOG("");
}

Model* ResourceManager::LoadModel(const std::string& file)
{
    auto count = modelsIds_.count(file);

    if (count > 0)
    {
        auto i = modelsIds_.at(file);
        DEBUG_LOG("Model already loaded, id : " + std::to_string(modelsIds_.at(file)));
        return models_[i].get();
    }

    auto model = loaderManager_.Load(file);
    if (not model)
        return nullptr;

    model->InitModel(file);
    modelsIds_.insert({model->GetFileName(), models_.size()});
    models_.push_back(std::move(model));
    gpuLoader_->AddObjectToGpuLoadingPass(models_.back().get());
    return models_.back().get();
}

void ResourceManager::AddModel(Model* model)
{
    models_.emplace_back(model);
    modelsIds_.insert({model->GetFileName(), models_.size() - 1});
    gpuLoader_->AddObjectToGpuLoadingPass(model);
}
Texture* ResourceManager::AddTexture(std::unique_ptr<Texture> texture)
{
    textures_.push_back(std::move(texture));
    return textures_.back().get();
}
void ResourceManager::DeleteTexture(uint32 id)
{
    auto iter = std::find_if(textures_.begin(), textures_.end(),
                             [id](const auto& texture) { return (texture->GetId() == id); });

    if (iter != textures_.end())
    {
        graphicsApi_.DeleteObject(id);
    }
    textures_.erase(iter);
}
}  // namespace GameEngine
