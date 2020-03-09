#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include "GameEngine/Resources/ITextureLoader.h"
#include "IResourceManager.hpp"
#include "Models/Model.h"
#include "Models/WBLoader/LoaderManager.h"

namespace GameEngine
{
class IGpuResourceLoader;

class ResourceManager : public IResourceManager
{
public:
    ResourceManager(GraphicsApi::IGraphicsApi& graphicsApi);
    ~ResourceManager() override;
    Model* LoadModel(const std::string& file) override;
    void AddModel(Model* model) override;
    void ReleaseModel(Model* model) override;
    inline Model* GetModel(uint32 id) override;
    inline IGpuResourceLoader& GetGpuResourceLoader() override;
    inline ITextureLoader& GetTextureLaoder() override;
    inline GraphicsApi::IGraphicsApi& GetGraphicsApi() override;
    Texture* AddTexture(std::unique_ptr<Texture>) override;
    void DeleteTexture(uint32 id);

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;

    std::vector<std::unique_ptr<Model>> models_;
    std::vector<std::unique_ptr<Texture>> textures_;

    std::unordered_map<std::string, uint32> modelsIds_;

    std::shared_ptr<IGpuResourceLoader> gpuLoader_;
    std::shared_ptr<ITextureLoader> textureLoader_;

    LoaderManager loaderManager_;
};

Model* ResourceManager::GetModel(uint32 id)
{
    return models_[id].get();
}

IGpuResourceLoader& ResourceManager::GetGpuResourceLoader()
{
    return *gpuLoader_;
}

ITextureLoader& ResourceManager::GetTextureLaoder()
{
    return *textureLoader_;
}

GraphicsApi::IGraphicsApi& ResourceManager::GetGraphicsApi()
{
    return graphicsApi_;
}
}  // namespace GameEngine
