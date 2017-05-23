#include "ParseUtils.h"

float Get::Float(std::string line)
{
	return std::stof(line);
}

int Get::Int(std::string line)
{
	return stoi(line);
}

glm::vec2 Get::Vector2d(std::string line, char prefix)
{
	float x = std::stof(line.substr(0, line.find_last_of(prefix)));
	float y = std::stof(line.substr(line.find_last_of(prefix) + 1));
	return glm::vec2(x, y);
}

glm::vec3 Get::Vector3d(std::string line, char prefix)
{
	float z = std::stof(line.substr(line.find_last_of(prefix) + 1));
	float x = std::stof(line.substr(0, line.find_first_of(prefix)));
	float y = std::stof(line.substr(line.find_first_of(prefix) + 1, line.find_last_of(prefix)));
	return glm::vec3(x, y, z);
}

bool Get::Boolean(std::string line)
{
	return std::stof(line) > 0 ? true : false;
}
