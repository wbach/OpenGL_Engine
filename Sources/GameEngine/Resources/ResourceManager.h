#pragma once
#include <memory>
#include <unordered_map>
#include <Mutex.hpp>
#include "GameEngine/Resources/ITextureLoader.h"
#include "IResourceManager.hpp"
#include "Models/Model.h"
#include "Models/WBLoader/LoaderManager.h"
#include "ResourceInfo.h"

namespace GameEngine
{
class ResourceManager : public IResourceManager
{
public:
    ResourceManager(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuResourceLoader);
    ~ResourceManager() override;

    Model* LoadModel(const File&) override;
    void AddModel(std::unique_ptr<Model>) override;
    void ReleaseModel(Model&) override;

    inline ITextureLoader& GetTextureLoader() override;
    inline IGpuResourceLoader& GetGpuResourceLoader() override;
    inline GraphicsApi::IGraphicsApi& GetGraphicsApi() override;

private:
    void DeleteMaterial(const Material&);

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    IGpuResourceLoader& gpuResourceLoader_;
    std::unique_ptr<ITextureLoader> textureLoader_;
    LoaderManager loaderManager_;

    std::unordered_map<std::string, ResourceInfo<Model>> models_;
    std::unordered_map<std::string, ResourceInfo<Texture>> textures_;

    uint32 unknowFileNameResourceId_;
    std::mutex modelMutex_;
};

IGpuResourceLoader& ResourceManager::GetGpuResourceLoader()
{
    return gpuResourceLoader_;
}

ITextureLoader& ResourceManager::GetTextureLoader()
{
    return *textureLoader_;
}

GraphicsApi::IGraphicsApi& ResourceManager::GetGraphicsApi()
{
    return graphicsApi_;
}
}  // namespace GameEngine
