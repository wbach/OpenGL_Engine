#pragma once
#include <string>
#include "glm/glm.hpp"

struct SGuiTextElement
{
	std::string text = "default text";
	glm::vec2 position;
	glm::vec3 colour;
	float m_size = 1;
	bool isActive = true;
};