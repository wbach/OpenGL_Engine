#pragma once
#include "../Model.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "MeshData.h"
#include <list>
#include <string>

namespace WBLoader
{
    class AbstractLoader
    {
    public:
		AbstractLoader(GameEngine::IGraphicsApiPtr graphicsApi);
        virtual void ParseFile(const std::string& filename)      = 0;
        virtual bool CheckExtension(const std::string& filename) = 0;
		virtual std::unique_ptr<CModel> Create();
	
	protected:
		std::list<WBLoader::Object> objects;

	private:
		void NormalizeMatrix(mat4& mat, float factor) const;
		float FindMaxFactor() const;

	private:
		GameEngine::IGraphicsApiPtr graphicsApi_;
    };

	typedef std::unique_ptr<WBLoader::AbstractLoader> AbstractLoaderPtr;
} // WBLoader
