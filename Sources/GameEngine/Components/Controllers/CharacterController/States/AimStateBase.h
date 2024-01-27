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
    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);
    void update(float);

    void onLeave(const WeaponStateEvent&);
    void onLeave(const AimStopEvent&);

protected:
    void setAnim();
    void stopMultiAnimation();
    void stopAnim();

protected:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
