#pragma once
#include "MeshData.h"
#include "IMeshLoader.h"
#include <list>

class CTextureLoader;

namespace WBLoader
{
	class ColladaDae : public IMeshLoader
	{
	public:
		ColladaDae(CTextureLoader& textureLodaer);
		virtual void ParseFile(const std::string& filename) override;
		virtual std::list<CMesh> CreateFinalMesh() override;
		virtual bool CheckExtension(const std::string& filename) override;
	private:
		CTextureLoader&	textureLodaer;
	};
}