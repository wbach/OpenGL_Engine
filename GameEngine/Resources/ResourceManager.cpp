#include "ResourceManager.h"
#include "Models/Assimp/AssimpModel.h"
#include "Models/WBLoader/MyModel.h"
#include "../Engine/Configuration.h"
#include "Logger/Log.h"

CResourceManager::CResourceManager()
    :  textureLoader(textures, openGlLoader)
{
}

CModel * CResourceManager::LoadModel(const std::string & file)
{
	auto count = modelsIds.count(EngineConf_GetFullDataPath(file));

	if (count > 0)
	{
		auto i = modelsIds[EngineConf_GetFullDataPath(file)];
		Log("Model already loaded, id : " + std::to_string(modelsIds[EngineConf_GetFullDataPath(file)]));
		return models[i].get();
	}

	
 //   for (const auto& model : models)
	//{
	//	auto filename_exist = EngineConf_GetOrginFilePath(model->GetFileName());
 //       if (filename_exist == file)
 //           return model.get();
	//}

	//CAssimModel  CMyModel
	auto model = new CMyModel(textureLoader);
    models.emplace_back(model);
	model->InitModel(EngineConf_GetFullDataPathAddToRequierd(file));
	modelsIds[model->GetFileName()] = models.size() - 1;
    return model;
}

void CResourceManager::AddModel(CModel * model) 
{ 
	models.emplace_back(model);
	modelsIds[model->GetFileName()] = models.size() - 1;
	openGlLoader.AddObjectToOpenGLLoadingPass(model);
}
