#pragma once
#include "MeshData.h"
#include "IMeshLoader.h"
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
		enum ArrayType
		{
			POSITIONS,
			NORMALS,
			UNKNOWN
		};

	protected:
		void ProccesColladaNode(const rapidxml::xml_document<char>& document);
		void ProccesLibraryGeometryNode(rapidxml::xml_node<char>* root);
		void ProccesGeometryNode(rapidxml::xml_node<char>* root);
		void ProccesMeshNode(rapidxml::xml_node<char>* root);
		void ProccesMeshSourceNode(rapidxml::xml_node<char>* root);
		ArrayType GetArrayType(const std::string& str) const;
	protected:
		std::string fileData;
		CTextureLoader&	textureLodaer;
	};
}