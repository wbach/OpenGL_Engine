#pragma once
#include "Types.h"

namespace GameEngine
{
const int MAX_INSTANCES = 100;

struct PerInstances
{
    mat4 transformationMatrixes[MAX_INSTANCES];
};
}  // namespace GameEngine
