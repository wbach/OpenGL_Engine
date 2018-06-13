#include "LoaderManager.h"
#include "Collada/Collada.h"
#include "WaveFront/WaveFrontObj.h"
#include "Terrain/TerrainMeshLoader.h"

#include "GameEngine/Engine/Configuration.h"

#include "Logger/Log.h"
#include "ParseUtils.h"
#include <sstream>
#include <algorithm>

LoaderManager::LoaderManager(CTextureLoader & textureloader)
	: textureloader(textureloader)
{
	loaders_.emplace_back(new WBLoader::WaveFrontObjLoader(textureloader));
	loaders_.emplace_back(new WBLoader::ColladaDae(textureloader));
	loaders_.emplace_back(new WBLoader::TerrainMeshLoader(textureloader));
}

std::unique_ptr<CModel> LoaderManager::Load(const std::string& file_name)
{
	auto extension = Utils::GetFileExtension(file_name);
	
	auto loaderPtr = GetLoader(extension);
	
	if (loaderPtr == nullptr)
	{
		Error("Try parse unkonwn file extension : " + extension);
		return nullptr;
	}

	loaderPtr->Parse(file_name);
	return loaderPtr->Create();
}

LoaderManager::~LoaderManager()
{
}

WBLoader::AbstractLoader* LoaderManager::GetLoader(const std::string& extension)
{
	for (auto& loader : loaders_)
	{
		if (loader->CheckExtension(extension))
			return loader.get();
	}
	return nullptr;
}
