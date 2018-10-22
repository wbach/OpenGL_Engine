#pragma once
#include "Types.h"
#include "GameEngine/Api/IGraphicsApi.h"

class CModel;
class COpenGLLoader;
class CTextureLoader;

namespace GameEngine
{
class IResourceManager
{
public:
    virtual ~IResourceManager()
    {
    }
    virtual CModel* LoadModel(const std::string& file) = 0;
    virtual void AddModel(CModel* model)               = 0;
    virtual CModel* GetModel(uint32 id)                = 0;
    virtual COpenGLLoader& GetOpenGlLoader()           = 0;
    virtual CTextureLoader& GetTextureLaoder()         = 0;
    virtual IGraphicsApiPtr GetGraphicsApi()           = 0;
};
}  // GameEngine
