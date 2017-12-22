#pragma once
#include "Types.h"
#include <list>

struct Join
{
	uint32 id;
	mat4 transform;
	std::string name;
	std::list<Join> chlidren;
};