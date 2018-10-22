#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include "IResourceManager.hpp"
#include "Models/Model.h"
#include "Models/WBLoader/LoaderManager.h"
#include "OpenGLLoader.h"
#include "TextureLoader.h"

namespace GameEngine
{
class ResourceManager : public IResourceManager
{
public:
    ResourceManager(IGraphicsApiPtr graphicsApi);
    ~ResourceManager() override;
    CModel* LoadModel(const std::string& file) override;
    void AddModel(CModel* model) override;
    CModel* GetModel(uint32 id) override
    {
        return models[id].get();
    }
    COpenGLLoader& GetOpenGlLoader() override
    {
        return openGlLoader;
    }
    CTextureLoader& GetTextureLaoder() override
    {
        return textureLoader;
    }
    GameEngine::IGraphicsApiPtr GetGraphicsApi() override
    {
        return graphicsApi_;
    }

private:
    GameEngine::IGraphicsApiPtr graphicsApi_;

    std::vector<std::unique_ptr<CModel>> models;
    std::vector<std::unique_ptr<CTexture>> textures;

    std::unordered_map<std::string, uint32> modelsIds;

    COpenGLLoader openGlLoader;
    CTextureLoader textureLoader;

    LoaderManager loaderManager_;
};
} // GameEngine
