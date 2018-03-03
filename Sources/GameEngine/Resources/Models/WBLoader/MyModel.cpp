#include "MyModel.h"
#include "Collada/Collada.h"
#include "WaveFront/WaveFrontObj.h"
#include "Terrain/TerrainMeshLoader.h"

#include "GameEngine/Engine/Configuration.h"

#include "Logger/Log.h"
#include "ParseUtils.h"
#include <sstream>
#include <algorithm>


CMyModel::CMyModel(CTextureLoader & textureloader)
	: textureloader(textureloader)
{
	meshLoaders.emplace_back(new WBLoader::WaveFrontObjLoader(textureloader));
	meshLoaders.emplace_back(new WBLoader::ColladaDae(textureloader));
	meshLoaders.emplace_back(new WBLoader::TerrainMeshLoader(textureloader));
}

CMyModel::~CMyModel()
{
    Log("Destructor my model : " + filename);
}

void CMyModel::InitModel(const std::string & file_name)
{
	CModel::InitModel(file_name);

	auto extension = Utils::GetFileExtension(file_name);
	
	auto IMeshLoader = GetActiveLoader(extension);
	
	if (IMeshLoader == nullptr)
	{
		Error("Try parse unkonwn file extension : " + extension);
		return;
	}

	IMeshLoader->ParseFile(file_name);
	for (auto& mesh : IMeshLoader->CreateFinalMesh())
		AddMesh(mesh);

	Log(file_name + " succesful loaded");
}

void CMyModel::OpenGLLoadingPass()
{
	CModel::OpenGLLoadingPass();
}

WBLoader::AbstractMeshLoader* CMyModel::GetActiveLoader(const std::string & extension)
{
	for (auto& loader : meshLoaders)
	{
		if (loader->CheckExtension(extension))
			return loader.get();
	}
	return nullptr;
}
