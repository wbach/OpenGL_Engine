#include "DrawArrowRunState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowRunState::DrawArrowRunState(FsmContext &context)
    : DrawArrowStateBase(context)
    , MoveStateBase{context, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
    , context_{context}
{
}
void DrawArrowRunState::onEnter()
{
    context_.multiAnimations = true;
}

void DrawArrowRunState::onEnter(const DrawArrowEvent &event)
{
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowRunState::onEnter(const RunForwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowRunState::onLeave(const AimStopEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowRunState::onLeave(const WeaponStateEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowRunState::update(float dt)
{
    MoveStateBase::update(dt);
}
}  // namespace Components
}  // namespace GameEngine
