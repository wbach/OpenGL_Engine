#include "PhysicsApiMock.h"

std::optional<RayHit> GameEngine::PhysicsApiMock::RayTest(const vec3&, const vec3&) const
{
	return std::optional<RayHit>();
}
