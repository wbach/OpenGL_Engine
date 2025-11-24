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
    timer.Reset();
}
void RotateableRunState::update(const MouseMoveEvent& event)
{
    timer.Reset();
    StateBase::updatePitchYaw(event);
}
void RotateableRunState::update()
{
    if (timer.GetTimeMilliseconds() > 2000)
    {
        timer.Reset();
        pushEventToQueue(MouseInactivityEvent{});
        return;
    }

    StateBase::cameraUpdate();
}
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
