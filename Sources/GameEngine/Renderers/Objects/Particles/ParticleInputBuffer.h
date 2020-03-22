#pragma once
#include <Types.h>

namespace GameEngine
{
struct ParticleInputBuffer
{
    AlignWrapper<mat4> projectionMatrix;
    AlignWrapper<float> textureNumberOfRows;
};
}  // namespace GameEngine