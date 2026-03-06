#pragma once
#include <Types.h>
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Physics
{
struct RayHit
{
    GameObject& gameObject;
    vec3 pointWorld;
    vec3 normalWorld;
    IdType rigidbodyId;
};
}  // namespace Physics
}  // namespace GameEngine
