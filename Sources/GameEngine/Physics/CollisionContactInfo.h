#pragma once
#include <Types.h>

namespace GameEngine
{
namespace Physics
{
struct CollisionContactInfo
{
    vec3 pos1;
    vec3 pos2;
    uint32 rigidbodyId1;
    uint32 rigidbodyId2;
    vec3 normal;
};
}  // namespace Physics
}  // namespace GameEngine
