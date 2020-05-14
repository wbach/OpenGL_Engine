#pragma once
#include <vector>
#include "Types.h"
#include <GL/glew.h>
#include <GraphicsApi/RenderType.h>

namespace OpenGLApi
{
struct OpenGLMesh
{
    bool useIndiecies = false;
    uint32 vao;
    VboMap vbos;
    uint32 patches;
    VboMap attributes;
    GLsizei vertexCount;
    uint32 instancesCount;
    GraphicsApi::RenderType renderType;
};
}  // namespace OpenGLApi
