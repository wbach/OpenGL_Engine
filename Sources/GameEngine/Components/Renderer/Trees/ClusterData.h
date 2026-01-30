#pragma once
#include <Types.h>

namespace GameEngine
{
struct ClusterData
{
    AlignWrapper<vec4> center;
    AlignWrapper<vec4> size;
};
}  // namespace GameEngine
