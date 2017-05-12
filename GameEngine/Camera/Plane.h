#pragma once
#include "glm/glm.hpp"

class CPlane
{
public:
	void Normalize()
	{
		float scale = 1.f / glm::length(m_Normal);
		m_Normal.x *= scale;
		m_Normal.y *= scale;
		m_Normal.z *= scale;
		m_DistanceToOrgin *= scale;
	}

public:
	glm::vec3 m_Normal; // The normal
	float m_DistanceToOrgin;  // The "distance" to the origin.
};