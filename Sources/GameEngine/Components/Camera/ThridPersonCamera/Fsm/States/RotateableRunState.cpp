#include "RotateableRunState.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
RotateableRunState::RotateableRunState(Context& context)
    : StateBase(context, context.cameraPositions.run)
{
}
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
