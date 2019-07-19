#include "MeshData.h"
#include <algorithm>
#include <numeric>
#include "GraphicsApi/MeshRawData.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace WBLoader
{
int FindIndex(const std::list<wb::vec3i>& vertexes, const wb::vec3i& v)
{
    auto it    = std::find(vertexes.begin(), vertexes.end(), v);
    auto index = std::distance(vertexes.begin(), it);

    if (it != vertexes.end())
        return index;
    else
        return -1;
}

wb::optional<int32> FindIndexFast(std::unordered_map<wb::vec3i, int32>& vertexes, const wb::vec3i& v)
{
    auto it = vertexes.find(v);
    if (it == vertexes.end())
        return wb::optional<int32>();
    return it->second;
}

void AddToBuffer(std::vector<float>& buffer, const vec3& v)
{
    buffer.push_back(v.x);
    buffer.push_back(v.y);
    buffer.push_back(v.z);
}

void AddToBuffer(std::vector<float>& buffer, const vec2& v)
{
    buffer.push_back(v.x);
    buffer.push_back(v.y);
}

void AddToBuffer(std::vector<int32>& buffer, const vec3i& v)
{
    buffer.push_back(v.x);
    buffer.push_back(v.y);
    buffer.push_back(v.z);
}

void AddToBuffer(std::vector<int32>& buffer, const vec4i& v)
{
    buffer.push_back(v.x);
    buffer.push_back(v.y);
    buffer.push_back(v.z);
    buffer.push_back(v.w);
}

void AddToBuffer(std::vector<float>& buffer, const vec4& v)
{
    buffer.push_back(v.x);
    buffer.push_back(v.y);
    buffer.push_back(v.z);
    buffer.push_back(v.w);
}

void minMax(float v, float& min, float& max)
{
    if (v < min)
        min = v;

    if (v > max)
        max = v;
}

void ScaleJoint(Animation::Joint& j, float f)
{
    j.transform *= glm::scale(vec3(1.f / f));

    for (auto& child : j.children)
    {
        ScaleJoint(child, f);
    }
}

void computeTangentBasis(std::vector<VertexBuffer>& vertexBuffer)
{
    for (uint32 i = 0; i < vertexBuffer.size(); i += 3)
    {
        // Shortcuts for vertices
        vec3& v0 = vertexBuffer[i + 0].position;
        vec3& v1 = vertexBuffer[i + 1].position;
        vec3& v2 = vertexBuffer[i + 2].position;

        // Shortcuts for UVs
        vec2& uv0 = vertexBuffer[i + 0].uvs;
        vec2& uv1 = vertexBuffer[i + 1].uvs;
        vec2& uv2 = vertexBuffer[i + 2].uvs;

        // Edges of the triangle : postion delta
        vec3 deltaPos1 = v1 - v0;
        vec3 deltaPos2 = v2 - v0;

        // UV delta
        vec2 deltaUV1 = uv1 - uv0;
        vec2 deltaUV2 = uv2 - uv0;

        float r        = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        vec3 tangent   = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

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
        vec3& n = vertexBuffer[i].normal;
        vec3& t = vertexBuffer[i].tangents;
        vec3& b = vertexBuffer[i].bitangents;

        // Gram-Schmidt orthogonalize
        t = glm::normalize(t - n * glm::dot(n, t));

        // Calculate handedness
        if (glm::dot(glm::cross(n, t), b) < 0.0f)
        {
            t = t * -1.0f;
        }
    }
}

void IndexinVBO(std::vector<VertexBuffer>& buffer, GraphicsApi::MeshRawData& data)
{
    computeTangentBasis(buffer);
    std::unordered_map<wb::vec3i, int32> out_indexes;
    data.indices_.reserve(buffer.size());

    for (auto& v : buffer)
    {
        auto optI = FindIndexFast(out_indexes, v.indexes);
        if (optI)
        {
            const auto& i = optI.constValue();
            data.indices_.push_back(i);

            auto index  = 3 * i;
            auto index2 = index + 1;
            auto index3 = index + 2;

            data.tangents_[index]  = v.tangents.x;
            data.tangents_[index2] = v.tangents.y;
            data.tangents_[index3] = v.tangents.z;

            data.bitangents_[index]  = v.bitangents.x;
            data.bitangents_[index2] = v.bitangents.y;
            data.bitangents_[index3] = v.bitangents.z;
        }
        else
        {
            AddToBuffer(data.positions_, v.position);
            AddToBuffer(data.normals_, v.normal);
            AddToBuffer(data.textCoords_, v.uvs);
            AddToBuffer(data.tangents_, v.tangents);
            AddToBuffer(data.bitangents_, v.bitangents);

            std::vector<JointInfo> joints;
            joints.reserve(GraphicsApi::MAX_BONES_PER_VERTEX);
            float sumWeights = 0.0f;
            if (not v.jointInfo.empty())
            {
                for (size_t jointIndex = 0; jointIndex < GraphicsApi::MAX_BONES_PER_VERTEX; ++jointIndex)
                {
                    if (jointIndex < v.jointInfo.size())
                    {
                        joints.push_back(v.jointInfo[jointIndex]);
                        sumWeights += v.jointInfo[jointIndex].weight;
                    }
                    else
                    {
                        joints.push_back({0, 0});
                    }
                }
            }

            for (auto& joint : joints)
            {
                data.joinIds_.push_back(joint.id);
                data.bonesWeights_.push_back(joint.weight / sumWeights);
            }

            auto newIndex          = out_indexes.size();
            out_indexes[v.indexes] = newIndex;
            data.indices_.push_back(newIndex);
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
}  // namespace WBLoader
}  // namespace GameEngine
