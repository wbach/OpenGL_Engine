#pragma once
#include <vector>

#include "Types.h"
#include "Vertex.h"


namespace GraphicsApi::Dx11
{
typedef uint32 Index;

struct Vao
{
    std::vector<Vertex> vertexes_;
    std::vector<Index> indices_;
};
}  // namespace GraphicsApi::Dx11
