#pragma once
#include <Utils/Fsm/Actions.h>

#include "DialogEvents.h"

namespace GameEngine
{
class TransitionState;
struct DialogContext;

class IdleState : public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                      Utils::StateMachine::On<StartRequested, Utils::StateMachine::TransitionTo<TransitionState>>,
                      Utils::StateMachine::On<StartInputWaiting, Utils::StateMachine::TransitionTo<TransitionState>>>
{
public:
    IdleState(DialogContext&);

    void onEnter();
    void onEnter(const EndDialog&);
    void onLeave();

private:
    void initGui();

private:
    DialogContext& dialogContext;
};
}  // namespace GameEngine
