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
    uint32 vao{0};
    VboMap vbos;
    uint32 patches{0};
    VboMap attributes;
    GLsizei vertexCount{0};
    uint32 instancesCount{0};
    GraphicsApi::RenderType renderType;
    int64 sizeInBytes{0};
};
}  // namespace OpenGLApi
