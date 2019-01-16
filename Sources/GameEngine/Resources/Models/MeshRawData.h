#pragma once
#include "Types.h"

namespace GameEngine
{
struct MeshRawData
{
    FloatAttributeVec positions_;
    FloatAttributeVec textCoords_;
    FloatAttributeVec normals_;
    FloatAttributeVec tangents_;
    FloatAttributeVec bitangents_;
    FloatAttributeVec bonesWeights_;
    IndicesVector indices_;
    JointsVector joinIds_;
    std::vector<mat4> instancedMatrixes_;
};
} // namespace GameEngine
