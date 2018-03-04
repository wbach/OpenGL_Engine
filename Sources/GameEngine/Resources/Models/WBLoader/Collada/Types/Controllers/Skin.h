#pragma once
#include "Types.h"
#include "Joints.h"
#include "VertexWeights.h"
#include "../Common/Source.h"
#include <unordered_map>

namespace GameEngine
{
	namespace Collada
	{
		struct Skin
		{
			Joitns joitns_;
			std::string source;
			mat4 bindShapeMatrix_;
			VertexWeights vertexWeights_;
			std::unordered_map<std::string, Source> sources_;
		};
	}
}
