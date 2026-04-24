#pragma once
#include <cstdint>

namespace GameEngine
{
enum class Layer : uint32_t
{
    Default   = 1 << 0,
    Terrain   = 1 << 1,
    Obstacle  = 1 << 2,
    Player    = 1 << 3,
    NoNavMesh = 1 << 4
};
}
