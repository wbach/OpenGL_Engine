#pragma once
#include "../MeshData.h"
#include "../AbstractLoader.h"
#include "Types/ColladaData.h"
#include "Xml/ColladaCommon.h"

class CTextureLoader;

namespace WBLoader
{
	class ColladaDae : public AbstractLoader
	{
	public:
		ColladaDae(CTextureLoader& textureLodaer);
		virtual void ParseFile(const std::string& filename) override;
		virtual bool CheckExtension(const std::string& filename) override;

	private:
		void ConstructModel();
		void FillAnimationData();
		void NewGeometry(const GameEngine::Collada::Geometry&);
		void NewMesh(const GameEngine::Collada::Mesh&, const std::string& geometryName);
		void GetVec2ArrayFromDataSource(const GameEngine::Collada::Mesh& mesh, const std::string& sourceId, std::vector<vec2>& v);
		void GetVec3ArrayFromDataSource(const GameEngine::Collada::Mesh& mesh, const std::string& sourceId, std::vector<vec3>& v);
		void PrepareMeshData(const GameEngine::Collada::Mesh& colladaMesh, Mesh& newMesh, const GameEngine::Collada::Input& input);
		GameEngine::Collada::DataTypes GetSourceType(const GameEngine::Collada::Source& source) const;
		void CreateSkeleton(const GameEngine::Collada::Node& node, GameEngine::Animation::Joint & joint, const std::unordered_map<std::string, uint32 >& joints);
		void FillAnimator(GameEngine::Animation::Animator& animator, const std::unordered_map<std::string, uint32 >& joints);

	private:
		void ApplyMaterials(SMaterial& material, const std::string& materialId);
		GameEngine::Collada::ObjectType GetType(const std::string&);
		const GameEngine::Collada::Node& GetNode(const std::string& node_id);
		const GameEngine::Collada::Node& GetNode(const GameEngine::Collada::Node& node, const std::string& node_id);

	private:
		GameEngine::Collada::ColladaData data_;
		std::unordered_map<std::string, Mesh*> idToMeshMap_;
		CTextureLoader& textureLodaer_;
	};
}