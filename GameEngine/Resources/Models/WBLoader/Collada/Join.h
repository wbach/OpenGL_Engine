#pragma once
#include "Types.h"
#include <list>

struct Join
{
	std::list<Join> chlidren;
	uint32 id;
	mat4 transform;
};