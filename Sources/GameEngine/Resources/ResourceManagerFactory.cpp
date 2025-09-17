#include "ResourceManagerFactory.h"

#include "Models/WBLoader/ModelLoaderFactory.h"
#include "ResourceManager.h"
#include "TextureLoader.h"

namespace GameEngine
{
ResourceManagerFactory::ResourceManagerFactory(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuResourceLoader)
    : graphicsApi{graphicsApi}
    , gpuResourceLoader{gpuResourceLoader}
{
}

std::unique_ptr<IResourceManager> ResourceManagerFactory::create()
{
    auto textureLoader      = std::make_unique<TextureLoader>(graphicsApi, gpuResourceLoader);
    auto modelLoaderFactory = std::make_unique<ModelLoaderFactory>(*textureLoader);

    return std::make_unique<ResourceManager>(graphicsApi, gpuResourceLoader, std::move(textureLoader),
                                             std::move(modelLoaderFactory));
}
}  // namespace GameEngine