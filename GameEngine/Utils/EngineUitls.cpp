#include "EngineUitls.h"

wb::vec2i Utils::CalculatePlaceInGird(const glm::vec3 &position, float gridSize)
{
   wb::vec2i position_in_grid;

   position_in_grid.x = static_cast<int>(position.x / gridSize);
   position_in_grid.y = static_cast<int>(position.z / gridSize);

   return position_in_grid;
}


wb::vec2i Utils::CalculatePlaceInGird(const glm::vec2 &position, float gridSize)
{
    return CalculatePlaceInGird(glm::vec3(position.x, 0, position.y), gridSize);
}
