#pragma once
#include <Types.h>

namespace GameEngine
{
float GetTerrainHeight(const std::vector<float>& data, float heightFactor, uint32 heightMapWidth, float offset, uint32 x, uint32 y);
}
