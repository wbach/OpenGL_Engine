#pragma once
#include <Common/Transform.h>
#include <Types.h>

#include "btBulletDynamicsCommon.h"

namespace GameEngine
{
namespace Physics
{
vec3 Convert(const btVector3&);
btVector3 Convert(const vec3&);
btQuaternion Convert(const Quaternion&);
Quaternion Convert(const btQuaternion&);
btTransform Convert(const common::Transform&);
btTransform Convert(const common::Transform&, const vec3&);
common::Transform Convert(const btTransform&);
}  // namespace Physics
}  // namespace GameEngine
