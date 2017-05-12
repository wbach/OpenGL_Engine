#pragma once
#include "../Model.h"
#include "../../../Utils/Types.h"
#include "../../TextureLoader.h"

namespace WBLoader
{
	struct VertexBuffer
	{
		wb::vec3i indexes;
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangents;
		glm::vec3 bitangents;
		glm::vec2 uvs;
		uint material_id;
	};

	struct Mesh
	{
		std::vector<VertexBuffer> vertexBuffer;

		std::vector<unsigned short> indices;
		std::vector<float> fpostions;
		std::vector<float> fuvs;
		std::vector<float> fnormal;
		std::vector<float> ftangents;
		std::vector<float> fbitangents;
		SMaterial material;

		int FindIndex(const std::list<wb::vec3i>& vertexes, const wb::vec3i& v);
		int FindIndexFast(std::map<wb::vec3i, unsigned short>& vertexes, const wb::vec3i& v);
		void AddVec3ToFloatBuffer(std::vector<float>& buffer, const glm::vec3& v);
		void AddVec2ToFloatBuffer(std::vector<float>& buffer, const glm::vec2& v);
		void IndexinVBO();
		void computeTangentBasis();
	};

	struct Object
	{
		std::string name;
		std::vector<Mesh> meshes;
	};
}
class CMyModel : public CModel
{
public:
	CMyModel(CTextureLoader& texture_lodaer);
	virtual			~CMyModel() override;
	virtual void	InitModel(const std::string& file_name) override;
private:
	void ParseObjFile(const std::string& file);
	void ReadMaterialFile(const std::string&file);
	void ProcesVertex(WBLoader::Object* object, const std::string& value);
	void ProcesNormal(WBLoader::Object* object, const std::string& value);
	void ProcesUVs(WBLoader::Object* object, const std::string& value);
	void ProcesFace(WBLoader::Object* object, WBLoader::Mesh* mesh, const std::string& value);
	void ProcesObject(WBLoader::Object*& object, const std::string& value);
	void ProcesMaterial(WBLoader::Object* object, WBLoader::Mesh*& mesh, const std::string& value);
	void AddFinalMeshes();	

	std::list<WBLoader::Object> objects;
	std::list<SMaterial> materials;

	std::vector<glm::vec3> vertex;
	std::vector<glm::vec2> text_coords;
	std::vector<glm::vec3> normals;

	CTextureLoader&	m_TextureLodaer;
};