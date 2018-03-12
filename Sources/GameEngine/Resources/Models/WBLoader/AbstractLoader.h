#pragma once
#include "../Model.h"
#include "MeshData.h"
#include <list>
#include <string>

namespace WBLoader
{
    class AbstractLoader
    {
    public:
        virtual void ParseFile(const std::string& filename)      = 0;
        virtual bool CheckExtension(const std::string& filename) = 0;
		virtual std::unique_ptr<CModel> Create();
	
	protected:
		std::list<WBLoader::Object> objects;

	private:
		void NormalizeMatrix(mat4& mat, float factor) const;

	private:
		float FindMaxFactor() const;
    };

	typedef std::unique_ptr<WBLoader::AbstractLoader> AbstractLoaderPtr;
} // WBLoader
