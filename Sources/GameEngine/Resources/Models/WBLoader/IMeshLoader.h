#pragma once
#include "../Mesh.h"
#include "MeshData.h"
#include <list>
#include <string>

namespace WBLoader
{
    class AbstractMeshLoader
    {
    public:
        virtual void ParseFile(const std::string& filename)      = 0;
        virtual bool CheckExtension(const std::string& filename) = 0;
		virtual std::list<CMesh> CreateFinalMesh();
	
	protected:
		std::list<WBLoader::Object> objects;

	private:
		void NormalizeMatrix(mat4& mat, float factor) const;

	private:
		float FindMaxFactor() const;
    };
} // WBLoader
