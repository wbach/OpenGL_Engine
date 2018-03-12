#pragma once
#include "../Model.h"
#include <vector>
#include <memory>

class CTextureLoader;

namespace WBLoader
{
	class AbstractLoader;
}

class LoaderManager
{
public:
	LoaderManager(CTextureLoader& textureloader);
	std::unique_ptr<CModel> Load(const std::string& file_name);
	~LoaderManager();

private:
	WBLoader::AbstractLoader* GetLoader(const std::string& extension);

private:
	typedef std::vector<std::unique_ptr<WBLoader::AbstractLoader>> LoadersVector;
	LoadersVector loaders_;
	CTextureLoader& textureloader;
};
