#include "DrawArrowState.h"

#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowState::DrawArrowState(FsmContext &context)
    : DrawArrowStateBase{context}
{
    context.animator.setPlayOnceForAnimationClip(context.animClipNames.drawArrow);
}
}  // namespace Components
}  // namespace GameEngine
