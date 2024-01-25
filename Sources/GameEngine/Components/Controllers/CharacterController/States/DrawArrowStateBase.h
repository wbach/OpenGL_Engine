#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
class AimState;
class ArmedIdleState;
class DisarmedIdleState;

class DrawArrowStateBase
{
public:
    DrawArrowStateBase(FsmContext&);
    void onEnter(const DrawArrowEvent&);
    void onEnter(const EndRotationEvent&);
    void update(float);

private:
    void setAnim();

private:
    FsmContext& context_;
    std::optional<IdType> subId_;
};
}  // namespace Components
}  // namespace GameEngine
#pragma once
