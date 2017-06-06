#pragma once
#include "ColladaDaeTypes.h"
#include "../MeshData.h"
#include "../IMeshLoader.h"
#include <list>

namespace rapidxml
{
	template<class T> class xml_node;
	template<class T> class xml_document;
}

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
	protected:
		void ProccesColladaNode(const rapidxml::xml_document<char>& document);
		void ProccesLibraryGeometryNode(rapidxml::xml_node<char>* root);
		WBLoader::Object ProccesGeometryNode(rapidxml::xml_node<char>* root);
		WBLoader::Mesh ProccesMeshNode(rapidxml::xml_node<char>* root);
		void ProccesMeshSourceNode(rapidxml::xml_node<char>* root, WBLoader::Mesh& out_mesh);
		void ProccesPolyListNode(rapidxml::xml_node<char>* root, WBLoader::Mesh& out_mesh);
		ColladaTypes::ArrayType GetArrayType(const std::string& str) const;

		std::vector<glm::vec2> GetVectors2dFromString(const std::string& str) const;
		std::vector<glm::vec3> GetVectors3dFromString(const std::string& str) const;
		std::vector<float> GetFloatsFromString(const std::string& str) const;
		std::vector<uint16> GetIntsFromString(const std::string& str) const;
	protected:
		std::list<WBLoader::Object> objects;

		std::string fileData;
		std::string parsingFileName;
		CTextureLoader&	textureLodaer;
	};
}