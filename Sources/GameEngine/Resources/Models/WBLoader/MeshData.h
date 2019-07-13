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
    bool needIndexing{true};
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

void IndexinVBO(std::vector<VertexBuffer>& buffer, GraphicsApi::MeshRawData& data);
}  // namespace WBLoader
}  // namespace GameEngine
