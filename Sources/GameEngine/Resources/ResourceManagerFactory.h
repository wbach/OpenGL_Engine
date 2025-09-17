#pragma once
#include "IResourceManagerFactory.h"

namespace GraphicsApi
{
class IGraphicsApi;
}

namespace GameEngine
{
class IGpuResourceLoader;

class ResourceManagerFactory : public IResourceManagerFactory
{
public:
    ResourceManagerFactory(GraphicsApi::IGraphicsApi&, IGpuResourceLoader&);
    std::unique_ptr<IResourceManager> create();

private:
    GraphicsApi::IGraphicsApi& graphicsApi;
    IGpuResourceLoader& gpuResourceLoader;
};
}  // namespace GameEngine