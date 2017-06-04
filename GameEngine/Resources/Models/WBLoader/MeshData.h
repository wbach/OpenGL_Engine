#pragma once
#include "../../../Utils/Types.h"
#include "../Material.h"
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

		void IndexinVBO();
		void computeTangentBasis();
	};

	struct Object
	{
		std::string name;
		std::vector<Mesh> meshes;
	};

	int FindIndex(const std::list<wb::vec3i>& vertexes, const wb::vec3i& v);
	int FindIndexFast(std::map<wb::vec3i, unsigned short>& vertexes, const wb::vec3i& v);
	void AddVec3ToFloatBuffer(std::vector<float>& buffer, const glm::vec3& v);
	void AddVec2ToFloatBuffer(std::vector<float>& buffer, const glm::vec2& v);	
}