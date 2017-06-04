#include "MeshData.h"

namespace WBLoader
{
	int FindIndex(const std::list<wb::vec3i>& vertexes, const wb::vec3i & v)
	{
		auto it = std::find(vertexes.begin(), vertexes.end(), v);
		auto index = std::distance(vertexes.begin(), it);

		if (it != vertexes.end())
			return index;
		else
			return -1;
	}

	int FindIndexFast(std::map<wb::vec3i, unsigned short>& vertexes, const wb::vec3i & v)
	{
		auto it = vertexes.find(v);
		if (it == vertexes.end())
			return -1;
		return it->second;
	}

	void AddVec3ToFloatBuffer(std::vector<float>& buffer, const glm::vec3 & v)
	{
		buffer.push_back(v.x);
		buffer.push_back(v.y);
		buffer.push_back(v.z);
	}

	void AddVec2ToFloatBuffer(std::vector<float>& buffer, const glm::vec2 & v)
	{
		buffer.push_back(v.x);
		buffer.push_back(v.y);
	}

	void Mesh::IndexinVBO()
	{
		computeTangentBasis();

		//std::list<wb::vec3i> out_indexes;
		std::map<wb::vec3i, unsigned short> out_indexes;
		for (auto& v : vertexBuffer)
		{
			//auto i = FindIndex(out_indexes, v.indexes);
			auto i = FindIndexFast(out_indexes, v.indexes);
			if (i >= 0)
			{
				indices.push_back(static_cast<unsigned short>(i));

				ftangents[3 * i] = v.tangents.x;
				ftangents[3 * i + 1] = v.tangents.y;
				ftangents[3 * i + 2] = v.tangents.z;


				fbitangents[3 * i] = v.bitangents.x;
				fbitangents[3 * i + 1] = v.bitangents.y;
				fbitangents[3 * i + 2] = v.bitangents.z;

			}
			else
			{
				AddVec3ToFloatBuffer(fpostions, v.position);
				AddVec3ToFloatBuffer(fnormal, v.normal);
				AddVec2ToFloatBuffer(fuvs, v.uvs);
				AddVec3ToFloatBuffer(ftangents, v.tangents);
				AddVec3ToFloatBuffer(fbitangents, v.bitangents);

				auto newIndex = (unsigned short)out_indexes.size();
				//out_indexes.push_back(v.indexes);
				out_indexes[v.indexes] = newIndex;
				indices.push_back(newIndex);
			}
		}
	}

	void Mesh::computeTangentBasis()
	{
		for (uint i = 0; i < vertexBuffer.size(); i += 3) 
		{
			// Shortcuts for vertices
			glm::vec3 & v0 = vertexBuffer[i + 0].position;
			glm::vec3 & v1 = vertexBuffer[i + 1].position;
			glm::vec3 & v2 = vertexBuffer[i + 2].position;

			// Shortcuts for UVs
			glm::vec2 & uv0 = vertexBuffer[i + 0].uvs;
			glm::vec2 & uv1 = vertexBuffer[i + 1].uvs;
			glm::vec2 & uv2 = vertexBuffer[i + 2].uvs;

			// Edges of the triangle : postion delta
			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;

			// UV delta
			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

			// Set the same tangent for all three vertices of the triangle.
			// They will be merged later, in vboindexer.cpp
			vertexBuffer[i + 0].tangents = tangent;
			vertexBuffer[i + 1].tangents = tangent;
			vertexBuffer[i + 2].tangents = tangent;

			vertexBuffer[i + 0].bitangents = bitangent;
			vertexBuffer[i + 1].bitangents = bitangent;
			vertexBuffer[i + 2].bitangents = bitangent;
		}

		// See "Going Further"
		for (uint i = 0; i < vertexBuffer.size(); i += 1)
		{
			glm::vec3 & n = vertexBuffer[i].normal;
			glm::vec3 & t = vertexBuffer[i].tangents;
			glm::vec3 & b = vertexBuffer[i].bitangents;

			// Gram-Schmidt orthogonalize
			t = glm::normalize(t - n * glm::dot(n, t));

			// Calculate handedness
			if (glm::dot(glm::cross(n, t), b) < 0.0f) {
				t = t * -1.0f;
			}
		}
	}
}
