#include "MeshData.h"
#include <algorithm>
#include <numeric>
#include "GraphicsApi/MeshRawData.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace WBLoader
{
long FindIndex(const std::list<wb::vec3i>& vertexes, const wb::vec3i& v)
{
    auto it    = std::find(vertexes.begin(), vertexes.end(), v);
    auto index = std::distance(vertexes.begin(), it);

    return it != vertexes.end() ? index : -1;
}

std::optional<int32> FindIndexFast(std::unordered_map<wb::vec3i, int32>& vertexes, const wb::vec3i& v)
{
    auto it = vertexes.find(v);

    if (it == vertexes.end())
        return std::nullopt;
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

void ScaleJoint(Animation::Joint& j, float f)
{
    j.transform *= glm::scale(vec3(1.f / f));

    for (auto& child : j.children)
    {
        ScaleJoint(child, f);
    }
}

vec3 GetSmoothTanget(const vec3& perTriangleNormal, const vec3 perTriangleTangent, const vec3& perVertexNormal)
{
    if (glm::dot(perTriangleNormal, perVertexNormal) > 0.999999f and
        glm::dot(perTriangleNormal, perVertexNormal) < -0.999999f)
    {
        return perTriangleTangent;
    }

    Quaternion q;
    vec3 a = glm::cross(perTriangleNormal, perVertexNormal);
    q.x    = a.x;
    q.y    = a.y;
    q.z    = a.z;
    q.w    = sqrtf((glm::length(perVertexNormal) * glm::length(perVertexNormal)) *
                (glm::length(perTriangleNormal) * glm::length(perTriangleNormal))) +
          glm::dot(perTriangleNormal, perVertexNormal);

    return glm::normalize(q) * perTriangleTangent;
}

void minMax(float v, float& min, float& max)
{
    if (v < min)
        min = v;

    if (v > max)
        max = v;
}

void minMax(const vec3& v, vec3& min, vec3& max)
{
    minMax(v.x, min.x, max.x);
    minMax(v.y, min.y, max.y);
    minMax(v.z, min.z, max.z);
}

void Mesh::computeTriangleVectors()
{
    for (uint32 i = 0; i < vertexBuffer.size(); i += 3)
    {
        auto& vb0 = vertexBuffer[i];
        auto& vb1 = vertexBuffer[i + 1];
        auto& vb2 = vertexBuffer[i + 2];

        const auto& v0  = vb0.position;
        const auto& v1  = vb1.position;
        const auto& v2  = vb2.position;
        const auto& n0  = vb0.normal;
        const auto& n1  = vb1.normal;
        const auto& n2  = vb2.normal;
        const auto& uv0 = vb0.uvs;
        const auto& uv1 = vb1.uvs;
        const auto& uv2 = vb2.uvs;

        // Edges of the triangle : postion delta
        vec3 deltaPos1 = v1 - v0;
        vec3 deltaPos2 = v2 - v0;

        // UV delta
        vec2 deltaUV1 = uv1 - uv0;
        vec2 deltaUV2 = uv2 - uv0;

        float rm = deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x;

        if (not compare(rm, 0))
        {
            float r         = 1.f / rm;
            vec3 tangent    = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
            vec3 flatNormal = glm::normalize(glm::cross(deltaPos1, deltaPos2));

            if (glm::length(n0) > 0.f and glm::length(n1) > 0.f and glm::length(n2) > 0.f)
            {
                vb0.tangents = GetSmoothTanget(flatNormal, tangent, n0);
                vb1.tangents = GetSmoothTanget(flatNormal, tangent, n1);
                vb2.tangents = GetSmoothTanget(flatNormal, tangent, n2);
            }
            else
            {
                vb0.tangents = tangent;
                vb1.tangents = tangent;
                vb2.tangents = tangent;
                vb0.normal   = flatNormal;
                vb1.normal   = flatNormal;
                vb2.normal   = flatNormal;
            }

            vb0.bitangents = glm::cross(n0, vb0.tangents);
            vb1.bitangents = glm::cross(n1, vb1.tangents);
            vb2.bitangents = glm::cross(n2, vb2.tangents);
        }
        else
        {
            if (glm::length(n0) > 0.f and glm::length(n1) > 0.f and glm::length(n2) > 0.f)
            {
                vec3 flatNormal = glm::normalize(glm::cross(deltaPos1, deltaPos2));
                vb0.normal      = flatNormal;
                vb1.normal      = flatNormal;
                vb2.normal      = flatNormal;
            }
        }
    }
}

GraphicsApi::MeshRawData Mesh::createMeshRawData()
{
    computeTriangleVectors();

    std::unordered_map<vec3i, int32> out_indexes;

    GraphicsApi::MeshRawData data;
    data.indices_.reserve(vertexBuffer.size());

    for (auto& v : vertexBuffer)
    {
        auto maybeIndex = FindIndexFast(out_indexes, v.indexes);
        if (maybeIndex)
        {
            data.indices_.push_back(*maybeIndex);

            auto index  = 3 * static_cast<size_t>(*maybeIndex);
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
                data.joinIds_.push_back(static_cast<int32>(joint.id));
                data.bonesWeights_.push_back(joint.weight / sumWeights);
            }

            auto newIndex = out_indexes.size();
            out_indexes.insert({v.indexes, newIndex});
            data.indices_.push_back(static_cast<IndicesDataType>(newIndex));
        }
    }
    return data;
}

BoundingBox Mesh::getBoundingBox() const
{
    vec3 min(std::numeric_limits<float>::max());
    vec3 max(-std::numeric_limits<float>::max());

    for (auto& v : vertexBuffer)
    {
        minMax(v.position, min, max);
    }

    return BoundingBox(min, max);
}
}  // namespace WBLoader
}  // namespace GameEngine
