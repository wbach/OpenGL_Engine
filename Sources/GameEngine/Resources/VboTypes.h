#pragma once
#include <unordered_map>
#include "Types.h"

namespace GameEngine
{
enum class VertexBufferObjects
{
    INDICES = 0,
    POSITION,
    TEXT_COORD,
    NORMAL,
    TANGENT,
    WEIGHTS,
    JOINTS,
    TRANSFORM_MATRIX,
    TEXTURE_OFFSET,
    BLEND_FACTOR
};

typedef std::unordered_map<VertexBufferObjects, uint32> VboMap;
}  // namespace GameEngine
