#pragma once
#include "Types.h"
#include "GameEngine/Objects/RenderAble/Paricle/Particle.h"
#include <vector>

namespace GameEngine
{
	void SortHighToLow(std::vector<Particle>& list, const vec3& camPosition);
	void SortUpHighToLow(std::vector<Particle>& list, int i, const vec3& camPosition);
}
