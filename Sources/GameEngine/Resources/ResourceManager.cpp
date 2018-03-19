#include "ResourceManager.h"
#include "Models/Assimp/AssimpModel.h"
#include "Logger/Log.h"

CResourceManager::CResourceManager(GameEngine::IGraphicsApiPtr graphicsApi)
    : graphicsApi_(graphicsApi)
	, textureLoader(graphicsApi, textures, openGlLoader)
	, loaderManager_(textureLoader)
{
}

CResourceManager::~CResourceManager()
{
	Log(__FUNCTION__);
}

CModel* CResourceManager::LoadModel(const std::string & file)
{
	auto count = modelsIds.count(file);

	if (count > 0)
	{
		auto i = modelsIds[file];
		Log("Model already loaded, id : " + std::to_string(modelsIds[file]));
		return models[i].get();
	}

	auto model = loaderManager_.Load(file);
	model->InitModel(file);
	modelsIds[model->GetFileName()] = models.size();
    models.push_back(std::move(model));
	openGlLoader.AddObjectToOpenGLLoadingPass(models.back().get());
    return models.back().get();
}

void CResourceManager::AddModel(CModel * model) 
{ 
	models.emplace_back(model);
	modelsIds[model->GetFileName()] = models.size() - 1;
	openGlLoader.AddObjectToOpenGLLoadingPass(model);
}
