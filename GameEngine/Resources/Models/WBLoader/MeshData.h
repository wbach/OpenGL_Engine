#pragma once
#include "../Material.h"
#include "Types.h"
#include <glm/glm.hpp>
#include <vector>
#include <list>
#include <map>

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
		uint32 material_id;
	};

	struct MaterialLayer
	{
		glm::vec4 color;
		std::string textureName;
	};

	//struct Material
	//{
	//	MaterialLayer emmision;
	//	MaterialLayer ambient;
	//	MaterialLayer diffuse;
	//	MaterialLayer specular;
	//	float shininess;
	//	float indexOfRefraction;
	//};

	struct Mesh
	{
		std::string name;

		std::vector<glm::vec3> vertex;
		std::vector<glm::vec2> text_coords;
		std::vector<glm::vec3> normals;
		//Material ogrin_material;

		std::vector<VertexBuffer> vertexBuffer;

		std::vector<uint16> indices;
		std::vector<float> fpostions;
		std::vector<float> fuvs;
		std::vector<float> fnormal;
		std::vector<float> ftangents;
		std::vector<float> fbitangents;
		SMaterial material;

		void IndexinVBO();
		void computeTangentBasis();
	};

	struct Object
	{
		std::string name;
		std::vector<Mesh> meshes;
		glm::mat4 transformMatrix;
	};

	int FindIndex(const std::list<wb::vec3i>& vertexes, const wb::vec3i& v);
	int FindIndexFast(std::map<wb::vec3i, uint16>& vertexes, const wb::vec3i& v);
	void AddVec3ToFloatBuffer(std::vector<float>& buffer, const glm::vec3& v);
	void AddVec2ToFloatBuffer(std::vector<float>& buffer, const glm::vec2& v);	
}