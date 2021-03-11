#include "CommonActions.h"

#include "FsmContext.h"
#include "GameEngine/Components/Physics/Rigidbody.h"

namespace GameEngine
{
namespace Components
{
void moveRigidbody(FsmContext &context)
{
    moveRigidbody(context.rigidbody, context.moveDirection, context.moveSpeed);
}
void moveRigidbody(Rigidbody &rigidbody, const vec3 &moveDirection, float moveSpeed)
{
    auto targetVelocity = rigidbody.GetRotation() * moveDirection;
    targetVelocity      = glm::normalize(targetVelocity);
    targetVelocity *= moveSpeed;

    auto velocity       = rigidbody.GetVelocity();
    auto velocityChange = (targetVelocity - velocity);
    velocityChange.x    = glm::clamp(velocityChange.x, -moveSpeed, moveSpeed);
    velocityChange.z    = glm::clamp(velocityChange.z, -moveSpeed, moveSpeed);
    velocityChange.y    = 0;

    auto newVelocity = velocity + velocityChange;
    rigidbody.SetVelocity(newVelocity);
}
}  // namespace Components
}  // namespace GameEngine
