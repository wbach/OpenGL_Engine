#pragma once
#include <memory>
#include "GraphicsApi/IGraphicsApi.h"
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
    virtual ~IResourceManager()                           = default;
    virtual Model* LoadModel(const std::string& file)     = 0;
    virtual void AddModel(Model* model)                   = 0;
    virtual void ReleaseModel(Model* model)               = 0;
    virtual Model* GetModel(uint32 id)                    = 0;
    virtual IGpuResourceLoader& GetGpuResourceLoader()    = 0;
    virtual ITextureLoader& GetTextureLaoder()            = 0;
    virtual GraphicsApi::IGraphicsApi& GetGraphicsApi()   = 0;
    virtual Texture* AddTexture(std::unique_ptr<Texture>) = 0;
};
}  // namespace GameEngine
