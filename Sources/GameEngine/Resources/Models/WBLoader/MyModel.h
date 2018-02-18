#pragma once
#include "../Model.h"
#include <vector>
#include <memory>

class CTextureLoader;

namespace WBLoader
{
	class AbstractMeshLoader;
}

class CMyModel : public CModel
{
public:
	CMyModel(CTextureLoader& textureloader);
	virtual			~CMyModel() override;
	virtual void	InitModel(const std::string& file_name) override;
	virtual void OpenGLLoadingPass() override;
private:
	WBLoader::AbstractMeshLoader* GetActiveLoader(const std::string& extension);
private:
	typedef std::vector<std::unique_ptr<WBLoader::AbstractMeshLoader>> LoadersVector;
	LoadersVector meshLoaders;
	CTextureLoader& textureloader;
};
