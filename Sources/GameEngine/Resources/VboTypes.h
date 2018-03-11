#pragma once
#include "Types.h"
#include <unordered_map>

enum class VertexBufferObjects
{
	INDICES = 0,
	POSITION,
	TEXT_COORD,
	NORMAL,
	TANGENT,
	WEIGHTS,
	JOINTS
	//TRANSFORM_MATRIX,
};

typedef std::unordered_map<VertexBufferObjects, uint32> VboMap;
