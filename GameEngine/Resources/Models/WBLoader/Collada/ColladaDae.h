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
class CTexture;

namespace WBLoader
{
	typedef std::list<CMesh> CMeshList;
	typedef std::list<WBLoader::Object> ObjectsList;
	typedef std::map<std::string, CTexture*> TexturesMap;
	typedef std::map<std::string, SMaterial> MaterialMap;

	typedef std::vector<vec2> Vec2Vector;
	typedef std::vector<vec3> Vec3Vector;
	typedef std::vector<float> FloatVector;
	typedef std::vector<uint16> Uint16Vector;

	typedef rapidxml::xml_node<char> XMLNode;
	typedef rapidxml::xml_document<char> XMLDocument;

	class ColladaDae : public IMeshLoader
	{
	public:
		ColladaDae(CTextureLoader& textureLodaer);
		virtual void ParseFile(const std::string& filename) override;
		virtual CMeshList CreateFinalMesh() override;
		virtual bool CheckExtension(const std::string& filename) override;
	protected:
		void ProccesColladaNode(const XMLDocument& document);

		//Geometry
		void ProccesLibraryGeometryNode(XMLNode* root);		
		WBLoader::Object ProccesGeometryNode(XMLNode* root);
		WBLoader::Mesh ProccesMeshNode(XMLNode* root);
		void ProccesMeshSourceNode(XMLNode* root, WBLoader::Mesh& out_mesh);
		void ProccesPolyListNode(XMLNode* root, WBLoader::Mesh& out_mesh);

		//VisualScenes
		void ProccesLibraryVisualScenesNode(XMLNode* root);
		void ProccesVisualScebeNode(XMLNode* root);
		void ProccesVisualSceneNodeInNode(XMLNode* root);

		void ProccesVisualSceneNodeInNode(XMLNode* root, WBLoader::Object& object);
		//Materials
		void ProccesLibraryEffects(XMLNode* root);
		void ProccesEffect(XMLNode* root);
		void ProccesProfileCommon(XMLNode* root, const std::string& material_name);
		void ProccesTechnique(XMLNode* root, const std::string& material_name);
		void ProccesNewParam(XMLNode* root);
		void ProccesPhong(XMLNode* root, const std::string& material_name);
		void ProccesEmission(XMLNode* root, const std::string& material_name);
		void ProccesAmbient(XMLNode* root, const std::string& material_name);
		void ProccesDiffuse(XMLNode* root, const std::string& material_name);
		void ProccesSpecular(XMLNode* root, const std::string& material_name);
		void ProccesShininess(XMLNode* root, const std::string& material_name);
		void ProccesIndexOfRefraction(XMLNode* root, const std::string& material_name);

		//Textures
		void ProccesLibraryImages(XMLNode* root);
		void ProccesImage(XMLNode* root);

		std::string GetName(const std::string& str, const std::string& postfix) const;
		ColladaTypes::ArrayType GetArrayType(const std::string& str) const;

		mat4 GetMatrixFromString(const std::string& str) const;
		Vec2Vector GetVectors2dFromString(const std::string& str) const;
		Vec3Vector GetVectors3dFromString(const std::string& str) const;
		FloatVector GetFloatsFromString(const std::string& str) const;
		Uint16Vector GetIntsFromString(const std::string& str) const;
	protected:
		ObjectsList objects;
		TexturesMap texturesMap;
		MaterialMap materialMap;

		std::string fileData;
		std::string parsingFileName;
		CTextureLoader&	textureLodaer;
	};
}