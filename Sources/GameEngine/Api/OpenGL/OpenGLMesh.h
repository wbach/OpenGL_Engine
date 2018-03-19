#pragma once
#include "Types.h"
#include "GameEngine/Resources/VboTypes.h"
#include <vector>

namespace GameEngine
{
	struct OpenGLMesh
	{
		uint32 vao;
		VboMap vbos;
		uint32 patches;
		VboMap attributes;
		uint32 vertexCount;
		uint32 instancesCount;
	};
} // GameEngine
