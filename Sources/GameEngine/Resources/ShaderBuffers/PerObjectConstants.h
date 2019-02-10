#pragma once
#include "Types.h"

namespace GameEngine
{
struct PerObjectConstants
{
    AlignWrapper<float> UseBoneTransform;
    AlignWrapper<vec2> textureOffset;
};
}  // namespace GameEngine
