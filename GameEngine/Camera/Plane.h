#pragma once
#include "Types.h"

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
	vec3 normal; // The normal
	float distanceToOrgin;  // The "distance" to the origin.
};