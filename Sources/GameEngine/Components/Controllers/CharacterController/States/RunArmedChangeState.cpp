#include "RunArmedChangeState.h"

namespace GameEngine
{
namespace Components
{
RunArmedChangeState::RunArmedChangeState(FsmContext &context)
    : MoveStateBase{context, context.runSpeed, context.animClipNames.disarmed.run.forward,
                    context.animClipNames.disarmed.run.backward}
    , context_{context}
{
}
void RunArmedChangeState::onEnter(const WeaponStateEvent &)
{
}
}  // namespace Components
}  // namespace GameEngine
