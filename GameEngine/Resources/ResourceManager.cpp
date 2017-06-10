#include "ResourceManager.h"
#include "Models/Assimp/AssimpModel.h"
#include "Models/WBLoader/MyModel.h"
#include "../Engine/Configuration.h"

CResourceManager::CResourceManager()
    :  textureLoader(textures, openGlLoader)
{
}

CModel * CResourceManager::LoadModel(const std::string & file)
{
    for (const auto& model : models)
	{
		auto filename_exist = EngineConf_GetOrginFilePath(model->GetFileName());
        if (filename_exist == file)
            return model.get();
	}
	//CAssimModel  CMyModel
    models.emplace_back(new CMyModel(textureLoader));
    models.back()->InitModel(EngineConf_GetFullDataPath(file));
    return models.back().get();
}
