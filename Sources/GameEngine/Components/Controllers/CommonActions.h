#pragma once
#include <Types.h>

namespace GameEngine
{
namespace Components
{
class Rigidbody;
class FsmContext;

void moveRigidbody(FsmContext&);
void moveRigidbody(Rigidbody&, const vec3&, float);
}
}  // namespace GameEngine
