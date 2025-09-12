#pragma once
#include "Glm.h"
#include <memory>
#include "Types.h"

namespace Utils
{
vec2i CalculatePlaceInGird(const glm::vec2& position, float gridSize);
vec2i CalculatePlaceInGird(const glm::vec3& position, float gridSize);
int Calcualte1DindexInArray(const vec2i& position, int arraySize);
int Calcualte1DindexInArray(int x, int y, int arraySize);
vec2i CalcualeteEdgeMinMaxValueInGrid(int x, int range, int arraySize);
}  // namespace Utils
