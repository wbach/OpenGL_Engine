#pragma once
#include <Utils/Fsm/Actions.h>

#include "DialogEvents.h"
#include "Fsm/actions/Update.h"
#include "GameEngine/Scene/TweenTransform.h"

namespace GameEngine
{
class IdleState;
class ShowingSentence;
struct DialogContext;

class TransitionState
    : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                                       Utils::StateMachine::On<EndDialog, Utils::StateMachine::TransitionTo<IdleState>>,
                                       Utils::StateMachine::On<StartSentence, Utils::StateMachine::TransitionTo<ShowingSentence>>>
{
public:
    TransitionState(DialogContext&);

    void onEnter(const StartRequested&);
    void onEnter(const EndDialog&);

private:
    std::optional<TweenTransform> calculateCameraTarget(GameObject&, Components::DialogueComponent&);

private:
    DialogContext& dialogContext;
};
}  // namespace GameEngine
