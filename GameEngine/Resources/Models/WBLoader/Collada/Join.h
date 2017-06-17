#pragma once

#include "Types.h"

#include <glm/glm.hpp>
#include <list>

struct Join
{
	std::list<Join> chlidren;
	uint32 id;
};