#pragma once
#include <glm/glm.hpp>
#include <string>

namespace Get
{
	float Float(const std::string& line);
	int Int(const std::string& line);
	glm::vec2 Vector2d(const std::string& line, char prefix = 'x');
	glm::vec3 Vector3d(const std::string& line, char prefix = 'x');
	glm::vec4 Vector4d(const std::string& line, char prefix = 'x');
	bool Boolean(const std::string& line);
}
