#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
class AimStateBase
{
public:
    AimStateBase(FsmContext&);
    void onEnter(const AimStartEvent&);
    void onEnter(const EndRotationEvent&);
    void update(float);

protected:
    void setAnim();

protected:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
