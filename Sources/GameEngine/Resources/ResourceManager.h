#pragma once
#include "Models/Model.h"
#include "TextureLoader.h"
#include "OpenGLLoader.h"
#include "Models/WBLoader/LoaderManager.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include <vector>
#include <memory>
#include <unordered_map>

class CResourceManager
{
public:
	CResourceManager(GameEngine::IGraphicsApiPtr graphicsApi);
	~CResourceManager();
	CModel* LoadModel(const std::string& file);
	void AddModel(CModel * model);
    CModel* GetModel(uint32 id) { return models[id].get(); }
    COpenGLLoader& GetOpenGlLoader() { return openGlLoader; }
    CTextureLoader& GetTextureLaoder() { return textureLoader; }
	GameEngine::IGraphicsApiPtr GetGraphicsApi() { return graphicsApi_; }

private:
	GameEngine::IGraphicsApiPtr graphicsApi_;

    std::vector<std::unique_ptr<CModel>>    models;
    std::vector<std::unique_ptr<CTexture>>	textures;

	std::unordered_map<std::string, uint32> modelsIds;

    COpenGLLoader openGlLoader;
    CTextureLoader textureLoader;

	LoaderManager loaderManager_;
};
