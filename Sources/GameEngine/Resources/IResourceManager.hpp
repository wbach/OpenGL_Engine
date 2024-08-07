#pragma once
#include <memory>

#include "File.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "Models/WBLoader/LoadingParameters.h"
#include "Types.h"

namespace GameEngine
{
class Model;
class IGpuResourceLoader;
class ITextureLoader;
class Texture;

class IResourceManager
{
public:
    virtual ~IResourceManager() = default;

    virtual Model* LoadModel(const File&, const LoadingParameters& = DEFAULT_LOADING_PARAMETERS) = 0;
    virtual Model* AddModel(std::unique_ptr<Model>)                                              = 0;
    virtual void ReleaseModel(Model&)                                                            = 0;
    virtual IGpuResourceLoader& GetGpuResourceLoader()                                           = 0;
    virtual ITextureLoader& GetTextureLoader()                                                   = 0;
    virtual GraphicsApi::IGraphicsApi& GetGraphicsApi()                                          = 0;
    virtual void LockReleaseResources()                                                          = 0;
    virtual void UnlockReleaseResources()                                                        = 0;
};
}  // namespace GameEngine
