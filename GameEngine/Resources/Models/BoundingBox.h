#pragma once
#include "glm/glm.hpp"

struct BoundingBox
{
	glm::vec3	m_BoundingBoxMin,
				m_BoundingBoxMax,
				m_BoundingCenter,
				m_BoundingSize;
};