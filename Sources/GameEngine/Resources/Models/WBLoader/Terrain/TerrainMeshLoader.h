#pragma once
#include "../IMeshLoader.h"
#include "Types.h"

class CTextureLoader;

namespace WBLoader
{
	class  TerrainMeshLoader : public AbstractMeshLoader
	{
	public:
		TerrainMeshLoader(CTextureLoader& textureLodaer);
		virtual void ParseFile(const std::string& filename) override;
		virtual bool CheckExtension(const std::string& filename) override;
		virtual std::list<CMesh> CreateFinalMesh() override;
	
	private:
		void CreateTerrainVertexes(int x_start, int y_start, int width, int height);
		vec3 CalculateNormalMap(int x, int z);
		float GetHeight(int x, int y) const;
		void CreateMesh();

	private:
		CTextureLoader& textureLoader_;
		float heightMapResolution_;
		std::vector<uint16> indices_;
		std::vector<float> vertices_;
		std::vector<float> normals_;
		std::vector<float> tangens_;
		std::vector<float> textureCoords_;
		std::vector<float>* heights_;
	};
} // WBLoader