#include "RunArmedChangeState.h"

namespace GameEngine
{
namespace Components
{
RunArmedChangeState::RunArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.runSpeed, context.animClipNames.disarmed.run.forward,
                    context.animClipNames.disarmed.run.backward}
    , context_{context}
{
}
void RunArmedChangeState::onEnter(const WeaponStateEvent &)
{
}
void RunArmedChangeState::update(float)
{
}
}  // namespace Components
}  // namespace GameEngine
