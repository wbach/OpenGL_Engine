#pragma once
#include <vector>
#include "Types.h"
#include "Vertex.h"

namespace DirectX
{
typedef uint32 Index;

struct Vao
{
    std::vector<Vertex> vertexes_;
    std::vector<Index> indices_;
};
}  // namespace DirectX
