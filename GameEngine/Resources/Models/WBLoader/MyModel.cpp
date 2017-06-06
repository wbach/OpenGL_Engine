#include "MyModel.h"
#include "Collada/ColladaDae.h"
#include "WaveFront/WaveFrontObj.h"

#include "../../../Utils/ParseUtils.h"
#include "../../../Engine/Configuration.h"

#include <sstream>
#include <algorithm>


CMyModel::CMyModel(CTextureLoader & textureloader)
	: textureloader(textureloader)
{
	meshLoaders.emplace_back(new WBLoader::WaveFrontObjLoader(textureloader));
	meshLoaders.emplace_back(new WBLoader::ColladaDae(textureloader));
}

CMyModel::~CMyModel()
{
    Log("Destructor my model : " + filename);
}

void CMyModel::InitModel(const std::string & file_name)
{
	filename = file_name;

	auto extension = Utils::GetFileExtension(filename);
	
	auto IMeshLoader = GetActiveLoader(extension);
	
	if (IMeshLoader == nullptr)
	{
		Error("Try parse unkonwn file extension : " + extension);
		return;
	}

	IMeshLoader->ParseFile(filename);
	for (auto& mesh : IMeshLoader->CreateFinalMesh())
		AddMesh(mesh);

	CModel::InitModel(filename);
}

WBLoader::IMeshLoader* CMyModel::GetActiveLoader(const std::string & extension)
{
	for (auto& loader : meshLoaders)
	{
		if (loader->CheckExtension(extension))
			return loader.get();
	}
	return nullptr;
}
