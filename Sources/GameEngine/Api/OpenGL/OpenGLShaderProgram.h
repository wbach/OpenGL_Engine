#pragma once
#include "Types.h"
#include <vector>

namespace GameEngine
{
	struct OpenGLShaderProgram
	{
		uint32 id;
		std::string name;
		std::vector<uint32> shaderObjectsList;
	};
} // GameEngine
