#pragma once
#include <glm/glm.hpp>

class CPlane
{
public:
	void Normalize()
	{
		float scale = 1.f / glm::length(normal);
		normal.x *= scale;
		normal.y *= scale;
		normal.z *= scale;
		distanceToOrgin *= scale;
	}

public:
	glm::vec3 normal; // The normal
	float distanceToOrgin;  // The "distance" to the origin.
};