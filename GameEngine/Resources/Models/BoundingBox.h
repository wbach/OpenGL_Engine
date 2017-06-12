#pragma once
#include <glm/glm.hpp>

struct BoundingBox
{
    glm::vec3	min,
                max,
                center,
                size;
};
