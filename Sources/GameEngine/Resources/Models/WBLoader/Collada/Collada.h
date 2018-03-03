#pragma once
#include "../MeshData.h"
#include "../IMeshLoader.h"
#include "Types/ColladaData.h"
#include "Xml/ColladaCommon.h"

class CTextureLoader;

namespace WBLoader
{
	class ColladaDae : public AbstractMeshLoader
	{
	public:
		ColladaDae(CTextureLoader& textureLodaer);
		virtual void ParseFile(const std::string& filename) override;
		virtual bool CheckExtension(const std::string& filename) override;

	private:
		void ConstructModel();
		void NewGeometry(const GameEngine::Collada::Geometry&);
		void NewMesh(const GameEngine::Collada::Mesh&);
		void GetVec2ArrayFromDataSource(const GameEngine::Collada::Mesh& mesh, const std::string& sourceId, std::vector<vec2>& v);
		void GetVec3ArrayFromDataSource(const GameEngine::Collada::Mesh& mesh, const std::string& sourceId, std::vector<vec3>& v);
		void PrepareMeshData(const GameEngine::Collada::Mesh& colladaMesh, Mesh& newMesh, const GameEngine::Collada::Input& input);
		GameEngine::Collada::DataTypes GetSourceType(const GameEngine::Collada::Source& source) const;

	private:
		void ApplyMaterials(SMaterial& material, const std::string& materialId);

	private:
		GameEngine::Collada::ColladaData data_;
		CTextureLoader& textureLodaer_;
	};
}