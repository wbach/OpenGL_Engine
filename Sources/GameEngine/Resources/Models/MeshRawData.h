#pragma once
#include <vector>
#include "Types.h"

namespace GameEngine
{
typedef std::vector<float> FloatVec;
typedef std::vector<int32> Int32Vec;
typedef std::vector<uint16> UintVec;
typedef std::vector<uint8> Uint8Vec;

struct MeshRawData
{
    FloatVec positions_;
    FloatVec textCoords_;
    FloatVec normals_;
    FloatVec tangents_;
    FloatVec bitangents_;
    FloatVec bonesWeights_;
    UintVec indices_;
    Int32Vec joinIds_;
    std::vector<mat4> instancedMatrixes_;
};
} // namespace GameEngine
