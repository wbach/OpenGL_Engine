#pragma once
#include "Types.h"
#include <vector>
#include <unordered_map>

namespace GameEngine
{
const uint32 NUM_BONES_PER_VEREX = 4;

struct VertexBoneData
{
    uint32 ids[NUM_BONES_PER_VEREX];
    float weights[NUM_BONES_PER_VEREX];

    VertexBoneData();

    void AddBoneData(uint32 bone_id, float weight);
};
struct BoneInfo
{
    mat4 boneOffset          = mat4(0.f);
    mat4 finalTransformation = mat4(0.f);
};

struct BonesInfo
{
    std::vector<BoneInfo> boneInfo;
    std::vector<VertexBoneData> bones;
    std::unordered_map<std::string, uint32> boneMapping;  // maps a bone name to its index
    uint32 numBones = 0;
};
}  // namespace GameEngine
