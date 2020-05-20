#pragma once
#include <list>
#include <vector>
#include "../Material.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Animations/Joint.h"
#include "Types.h"
#include "optional.hpp"

namespace GraphicsApi
{
struct MeshRawData;
}  // namespace GraphicsApi

namespace GameEngine
{
namespace WBLoader
{
struct JointInfo
{
    uint32 id{0};
    float weight{1.f};
};
struct VertexBuffer
{
    vec3i indexes{0,0,0};
    vec2 uvs{0};
    vec3 position{0};
    vec3 normal{0};
    vec3 tangents{0};
    vec3 bitangents{0};
    uint32 material_id{0};
    std::vector<JointInfo> jointInfo;
};

struct MaterialLayer
{
    vec4 color{1.f};
    std::string textureName;
};

struct Mesh
{
    bool needIndexing{true};
    std::string name;
    Material material;
    std::vector<vec3> vertex;
    std::vector<vec2> text_coords;
    std::vector<vec3> normals;
    std::vector<VertexBuffer> vertexBuffer;
    std::unordered_map<uint32, std::vector<int>> vertexPlacesInVertexBuffer_;
    Animation::Joint skeleton_;
    std::unordered_map<std::string, Animation::AnimationClip> animationClips_;
    float GetScaleFactor() const;
};

struct Object
{
    std::string name;
    std::vector<Mesh> meshes;
    mat4 transformMatrix{1.f};
};

void IndexinVBO(std::vector<VertexBuffer>& buffer, GraphicsApi::MeshRawData& data);
}  // namespace WBLoader
}  // namespace GameEngine
