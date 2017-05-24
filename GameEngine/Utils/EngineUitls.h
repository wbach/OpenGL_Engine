#pragma once
#include "Types.h"
#include <glm/glm.hpp>
#include <memory>

namespace Utils
{
    wb::vec2i CalculatePlaceInGird(const glm::vec2& position, float gridSize);
    wb::vec2i CalculatePlaceInGird(const glm::vec3& position, float gridSize);
}
