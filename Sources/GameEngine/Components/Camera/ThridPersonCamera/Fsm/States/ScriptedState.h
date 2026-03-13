#pragma once
#include <Utils/Fsm/Actions.h>

#include "../Context.h"
#include "../ThridPersonCameraEvents.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
class ScriptedState
    : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                                       Utils::StateMachine::On<StopScriptedMode, Utils::StateMachine::BackToPreviousState>>
{
public:
    ScriptedState(Context& context);
    void onEnter();

private:
    Context& context;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
