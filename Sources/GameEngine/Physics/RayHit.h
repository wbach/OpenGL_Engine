#pragma once
#include <Types.h>

namespace GameEngine
{
namespace Physics
{
struct RayHit
{
    vec3 pointWorld;
    vec3 normalWorld;
    IdType rigidbodyId;
};
}  // namespace Physics
}  // namespace GameEngine
