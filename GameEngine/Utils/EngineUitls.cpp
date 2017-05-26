#include "EngineUitls.h"

wb::vec2i Utils::CalculatePlaceInGird(const glm::vec3 &position, float gridSize)
{
   wb::vec2i position_in_grid;

   position_in_grid.x = static_cast<int>(position.x / gridSize);
   position_in_grid.y = static_cast<int>(position.z / gridSize);

   return position_in_grid;
}

int Utils::Calcualte1DindexInArray(const wb::vec2i & position, int arraySize)
{
	return position.x + arraySize * position.y;
}

int Utils::Calcualte1DindexInArray(int x, int y, int arraySize)
{
	return x + arraySize * y;
}

wb::vec2i Utils::CalcualeteEdgeMinMaxValueInGrid(int x, int range, int arraySize)
{
	wb::vec2i out;
	out.x = x - range;
	if (out.x < 0) out.x = 0;

	out.y = x + range;
	if (out.y > arraySize - 1) out.y = arraySize - 1;
	return out;
}

wb::vec2i Utils::CalculatePlaceInGird(const glm::vec2 &position, float gridSize)
{
    return CalculatePlaceInGird(glm::vec3(position.x, 0, position.y), gridSize);
}
