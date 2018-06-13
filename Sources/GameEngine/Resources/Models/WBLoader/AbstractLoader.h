#pragma once
#include "../Model.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "MeshData.h"
#include <list>
#include <string>

class CTextureLoader;

namespace WBLoader
{
    class AbstractLoader
    {
    public:
		AbstractLoader(GameEngine::IGraphicsApiPtr graphicsApi, CTextureLoader& textureLoader);
		void Parse(const std::string& filename);
        virtual bool CheckExtension(const std::string& filename) = 0;
		virtual std::unique_ptr<CModel> Create();
	
	protected:
        virtual void ParseFile(const std::string& filename)      = 0;
		std::list<WBLoader::Object> objects;
		void CreateBinFile(const std::unique_ptr<CModel>& model);
		std::unique_ptr<CModel> CreateModel();
		std::unique_ptr<CModel> CreateModelFromBin();

	protected:
		CTextureLoader& textureLoader_;

	private:
		void NormalizeMatrix(mat4& mat, float factor) const;
		float FindMaxFactor() const;
		std::unique_ptr<CModel> ReadBinFile(const std::string& filename);

	private:
		GameEngine::IGraphicsApiPtr graphicsApi_;
		std::string fileName_;
		std::string filePath_;
		bool loadedFromBin_;
    };

	typedef std::unique_ptr<WBLoader::AbstractLoader> AbstractLoaderPtr;
} // WBLoader
