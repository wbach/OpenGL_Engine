#pragma once
#include <glm/glm.hpp>
#include <string>

namespace Get
{
	float Float(std::string line);
	int Int(std::string line);
	glm::vec2 Vector2d(std::string line, char prefix = 'x');
	glm::vec3 Vector3d(std::string line, char prefix = 'x');
	glm::vec4 Vector4d(std::string line, char prefix = 'x');
	bool Boolean(std::string line);
}
