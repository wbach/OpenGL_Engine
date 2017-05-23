#pragma once
#include "Models/Model.h"
#include "TextureLoader.h"
#include "OpenGLLoader.h"
#include <vector>
#include <memory>

class CResourceManager
{
public:
	CResourceManager();
	CModel* LoadModel(const std::string& file);
    void AddModel(CModel * model) { models.emplace_back(model); openGlLoader.AddObjectToOpenGLLoadingPass(model); }
    CModel* GetModel(uint id) { return models[id].get(); }
    COpenGLLoader& GetOpenGlLoader() { return openGlLoader; }
    CTextureLoader& GetTextureLaoder() { return textureLoader; }

private:
    std::vector<std::unique_ptr<CModel>>    models;
    std::vector<std::unique_ptr<CTexture>>	textures;

    COpenGLLoader openGlLoader;
    CTextureLoader textureLoader;
};
