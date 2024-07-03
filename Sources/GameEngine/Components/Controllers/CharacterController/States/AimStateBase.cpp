#include "AimStateBase.h"

#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimStateBase::AimStateBase(FsmContext &context, const std::optional<std::string> &jointGroupName)
    : AimingStateBase{context, jointGroupName, context.animClipNames.aimIdle}
{
}
void AimStateBase::onEnter(const AimStartEvent &)
{
    // /*DISABLED*/ DEBUG_LOG("onEnter AimStartEvent clip : " + context_.animClipNames.aimIdle);
    setAnim();
}
}  // namespace Components
}  // namespace GameEngine
