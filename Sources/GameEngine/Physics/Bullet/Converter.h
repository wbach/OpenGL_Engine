#pragma once
#include "Common/Transform.h"
#include "Types.h"
#include "btBulletDynamicsCommon.h"

namespace GameEngine
{
namespace Physics
{
vec3 Convert(const btVector3& v);
btVector3 Convert(const vec3& v);
btQuaternion Convert(const Quaternion& quat);
Quaternion Convert(const btQuaternion& quat);
btTransform Convert(common::Transform& transform);
}  // namespace Physics
}  // namespace GameEngine
