#include "DrawArrowRunAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowRunAndRotateState::DrawArrowRunAndRotateState(FsmContext& context)
    : DrawArrowStateBase(context, context.upperBodyGroupName)
    , MoveAndRotateStateBase{context, context.lowerBodyGroupName, context.runSpeed, context.animClipNames.armed.run,
                             context.animClipNames.armed.rotateLeft, context.animClipNames.armed.rotateRight}
    , context_{context}
{
}

void DrawArrowRunAndRotateState::onEnter(const DrawArrowEvent& event)
{
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowRunAndRotateState::onEnter(const ReloadArrowEvent& event)
{
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowRunAndRotateState::update(float dt)
{
    DrawArrowStateBase::update(dt);
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
}

void DrawArrowRunAndRotateState::onLeave(const AimStopEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowRunAndRotateState::onLeave(const WeaponStateEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowRunAndRotateState::onLeave(const SprintStateChangeEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
