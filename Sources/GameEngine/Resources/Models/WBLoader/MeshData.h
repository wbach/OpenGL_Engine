#pragma once
#include "../Material.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Animations/Joint.h"
#include "Types.h"
#include "optional.hpp"
#include <vector>
#include <list>

namespace GraphicsApi
{
    struct MeshRawData;
} // GraphicsApi

namespace GameEngine
{

namespace WBLoader
{
struct VertexBuffer
{
    vec3i indexes;
    vec2 uvs;
    vec3 position;
    vec3 normal;
    vec3 tangents;
    vec3 bitangents;
    uint32 material_id;
    wb::optional<vec3> weights;
    wb::optional<vec3i> jointIds;
};

struct MaterialLayer
{
    vec4 color;
    std::string textureName;
};

struct Mesh
{
    std::string name;
    Material material;
    std::vector<vec3> vertex;
    std::vector<vec2> text_coords;
    std::vector<vec3> normals;
    std::vector<VertexBuffer> vertexBuffer;
    std::unordered_map<uint32, std::vector<int>> vertexPlacesInVertexBuffer_;
    float GetScaleFactor() const;
    Animation::Joint skeleton_;
    std::unordered_map<std::string, Animation::AnimationClip> animationClips_;
};

struct Object
{
    std::string name;
    std::vector<Mesh> meshes;
    mat4 transformMatrix;
};

int FindIndex(const std::list<vec3i>& vertexes, const vec3i& v);
void AddVec3ToFloatBuffer(std::vector<float>& buffer, const vec3& v);
void AddVec2ToFloatBuffer(std::vector<float>& buffer, const vec2& v);
void AddVec3ToIntBuffer(std::vector<int32>& buffer, const vec3i& v);
void IndexinVBO(std::vector<VertexBuffer>& buffer, GraphicsApi::MeshRawData& data);
void computeTangentBasis(std::vector<VertexBuffer>& vertexBuffer);
}
} // namespace GameEngine
