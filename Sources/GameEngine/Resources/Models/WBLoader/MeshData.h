#pragma once
#include "../Material.h"
#include "../Animation/Joint.h"
#include "../Animation/Animator.h"
#include "Types.h"
#include <vector>
#include <list>
#include <map>

namespace WBLoader
{
	struct VertexBuffer
	{
		wb::vec3i indexes;
		vec3 position;
		vec3 normal;
		vec3 tangents;
		vec3 bitangents;
		vec3 weights;
		wb::vec3i jointIds;
		vec2 uvs;
		uint32 material_id;
	};

	struct MaterialLayer
	{
		vec4 color;
		std::string textureName;
	};

	struct Mesh
	{
		std::string name;

		std::vector<vec3> vertex;
		std::vector<vec2> text_coords;
		std::vector<vec3> normals;
		//Material ogrin_material;

		std::vector<VertexBuffer> vertexBuffer;
		std::unordered_map<uint32, std::vector<int>> vertexPlacesInVertexBuffer_;

		std::vector<uint16> indices;
		std::vector<int32> jointIds;
		std::vector<float> fpostions;
		std::vector<float> fuvs;
		std::vector<float> fnormal;
		std::vector<float> ftangents;
		std::vector<float> fbitangents;
		std::vector<float> bonesWeights;
		SMaterial material;

		void IndexinVBO();
		void computeTangentBasis();

		float GetScaleFactor() const;
		GameEngine::Animation::Joint rootJoint_;
		GameEngine::Animation::Animator animator_;
	};

	struct Object
	{
		std::string name;
		std::vector<Mesh> meshes;
		mat4 transformMatrix;
	};

	int FindIndex(const std::list<wb::vec3i>& vertexes, const wb::vec3i& v);
	int FindIndexFast(std::map<wb::vec3i, uint16>& vertexes, const wb::vec3i& v);
	void AddVec3ToFloatBuffer(std::vector<float>& buffer, const vec3& v);
	void AddVec2ToFloatBuffer(std::vector<float>& buffer, const vec2& v);
	void AddVec3ToIntBuffer(std::vector<int32>& buffer, const vec3i& v);
}
