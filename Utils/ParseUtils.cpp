#include "ParseUtils.h"

float Get::Float(const std::string& line)
{
	return std::stof(line);
}

int Get::Int(const std::string& line)
{
	return stoi(line);
}

glm::vec2 Get::Vector2d(const std::string& line, char prefix)
{
	float x = std::stof(line.substr(0, line.find_last_of(prefix)));
	float y = std::stof(line.substr(line.find_last_of(prefix) + 1));
	return glm::vec2(x, y);
}

glm::vec3 Get::Vector3d(const std::string& line, char prefix)
{
	float z = std::stof(line.substr(line.find_last_of(prefix) + 1));
	float x = std::stof(line.substr(0, line.find_first_of(prefix)));
	float y = std::stof(line.substr(line.find_first_of(prefix) + 1, line.find_last_of(prefix)));
	return glm::vec3(x, y, z);
}

glm::vec4 Get::Vector4d(const std::string& line, char prefix)
{
	auto l = line;
	float w = std::stof(line.substr(line.find_last_of(prefix) + 1));

	l = l.substr(0, line.find_last_of(prefix));

	float z = std::stof(line.substr(line.find_last_of(prefix) + 1));

	l = l.substr(0, line.find_last_of(prefix));

	float y = std::stof(line.substr(line.find_last_of(prefix) + 1));

	l = l.substr(0, line.find_last_of(prefix));

	float x = std::stof(line.substr(line.find_last_of(prefix) + 1));

	return glm::vec4(x, y, z, w);
}

bool Get::Boolean(const std::string& line)
{
	return std::stof(line) > 0 ? true : false;
}
