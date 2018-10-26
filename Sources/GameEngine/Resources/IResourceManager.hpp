#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "Types.h"

namespace GameEngine
{
class Model;
class IGpuResourceLoader;
class ITextureLoader;

class IResourceManager
{
public:
    virtual ~IResourceManager()
    {
    }
    virtual Model* LoadModel(const std::string& file)  = 0;
    virtual void AddModel(Model* model)                = 0;
    virtual Model* GetModel(uint32 id)                 = 0;
    virtual IGpuResourceLoader& GetGpuResourceLoader() = 0;
    virtual ITextureLoader& GetTextureLaoder()         = 0;
    virtual IGraphicsApiPtr GetGraphicsApi()           = 0;
};
}  // GameEngine
