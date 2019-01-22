#pragma once
#include <vector>
#include "Types.h"

namespace OpenGLApi
{
struct OpenGLMesh
{
    uint32 vao;
    VboMap vbos;
    uint32 patches;
    VboMap attributes;
    uint32 vertexCount;
    uint32 instancesCount;
};
}  // namespace OpenGLApi
