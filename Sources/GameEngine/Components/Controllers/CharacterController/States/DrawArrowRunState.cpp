#include "DrawArrowRunState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowRunState::DrawArrowRunState(FsmContext& context)
    : DrawArrowStateBase(context)
    , MoveStateBase{context, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
{
}
void DrawArrowRunState::update(float)
{
}
}  // namespace Components
}  // namespace GameEngine
