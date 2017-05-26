#include "Types.h"

std::string wb::to_string(const vec2i & v)
{
	return std::to_string(v.x) + " " + std::to_string(v.y);
}

std::string wb::to_string(const vec3i & v)
{
	return std::to_string(v.x) + " " + std::to_string(v.y) + " " + std::to_string(v.z);
}
