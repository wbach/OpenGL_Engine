#pragma once
#include "Color.h"
#include "Float.h"
#include "NewParam.h"
#include <unordered_map>
#include <string>

namespace GameEngine
{
	namespace Collada
	{
		struct Effect
		{
			std::string id;
			Color emission;
			Color ambient;
			Color diffuse;
			Color specular;
			Float shininess;
			Float indexOfRefraction;
			std::unordered_map<std::string, NewParam> newParams_;
		};
	}
}