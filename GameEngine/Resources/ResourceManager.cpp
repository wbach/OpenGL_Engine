#include "ResourceManager.h"
#include "Models/Assimp/AssimpModel.h"
#include "Models/MyOBJ/MyModel.h"

CResourceManager::CResourceManager()
    :  textureLoader(textures, openGlLoader)
{
}

CModel * CResourceManager::LoadModel(const std::string & file)
{
    for (const auto& model : models)
	{
        if (model->GetFileName().compare(file) == 0)
            return model.get();
	}
	//CAssimModel  CMyModel
    models.emplace_back(new CMyModel(textureLoader));
    models.back()->InitModel(file);
    return models.back().get();
}
