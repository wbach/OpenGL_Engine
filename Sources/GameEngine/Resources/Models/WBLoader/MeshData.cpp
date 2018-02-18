#include "MeshData.h"
#include <algorithm>
#include "Logger/Log.h"

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

	int FindIndexFast(std::map<wb::vec3i, uint16>& vertexes, const wb::vec3i & v)
	{
		auto it = vertexes.find(v);
		if (it == vertexes.end())
			return -1;
		return it->second;
	}

	void AddVec3ToFloatBuffer(std::vector<float>& buffer, const vec3 & v)
	{
		buffer.push_back(v.x);
		buffer.push_back(v.y);
		buffer.push_back(v.z);
	}

	void AddVec2ToFloatBuffer(std::vector<float>& buffer, const vec2 & v)
	{
		buffer.push_back(v.x);
		buffer.push_back(v.y);
	}

	void minMax(float v , float& min, float& max)
	{
		if (v < min)
			min = v;

		if (v > max)
			max = v;
	}

	void Mesh::Normalize(float factor)
	{
		for (auto& v : vertexBuffer)
		{
			v.position /= factor;
		}
	}

	void Mesh::IndexinVBO()
	{
		computeTangentBasis();

		std::map<wb::vec3i, uint16> out_indexes;		

		for (auto& v : vertexBuffer)
		{
			//auto i = FindIndex(out_indexes, v.indexes);
			auto i = FindIndexFast(out_indexes, v.indexes);
			if (i >= 0)
			{
				indices.push_back(static_cast<uint16>(i));

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

				auto newIndex = (uint16)out_indexes.size();
				//out_indexes.push_back(v.indexes);
				out_indexes[v.indexes] = newIndex;
				indices.push_back(newIndex);
			}
		}
	}

	void Mesh::computeTangentBasis()
	{
		for (uint32 i = 0; i < vertexBuffer.size(); i += 3) 
		{
			// Shortcuts for vertices
			vec3 & v0 = vertexBuffer[i + 0].position;
			vec3 & v1 = vertexBuffer[i + 1].position;
			vec3 & v2 = vertexBuffer[i + 2].position;

			// Shortcuts for UVs
			vec2 & uv0 = vertexBuffer[i + 0].uvs;
			vec2 & uv1 = vertexBuffer[i + 1].uvs;
			vec2 & uv2 = vertexBuffer[i + 2].uvs;

			// Edges of the triangle : postion delta
			vec3 deltaPos1 = v1 - v0;
			vec3 deltaPos2 = v2 - v0;

			// UV delta
			vec2 deltaUV1 = uv1 - uv0;
			vec2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
			vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

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
		for (uint32 i = 0; i < vertexBuffer.size(); i += 1)
		{
			vec3 & n = vertexBuffer[i].normal;
			vec3 & t = vertexBuffer[i].tangents;
			vec3 & b = vertexBuffer[i].bitangents;

			// Gram-Schmidt orthogonalize
			t = glm::normalize(t - n * glm::dot(n, t));

			// Calculate handedness
			if (glm::dot(glm::cross(n, t), b) < 0.0f) {
				t = t * -1.0f;
			}
		}
	}
	float Mesh::GetScaleFactor() const
	{
		vec3 min(std::numeric_limits<float>::max());
		vec3 max(-std::numeric_limits<float>::max());

		for (const auto& v : vertexBuffer)
		{
			minMax(v.position.x, min.x, max.x);
			minMax(v.position.y, min.y, max.y);
			minMax(v.position.z, min.z, max.z);
		}

		float lx = fabs(max.x - min.x);
		float ly = fabs(max.y - min.y);
		float lz = fabs(max.z - min.z);

		float maxD = lx;
		if (ly > maxD)
			maxD = ly;
		if (lz > maxD)
			maxD = lz;

		return maxD;
	}
}
