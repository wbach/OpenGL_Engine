#pragma once
#include <Utils/Fsm/Actions.h>
#include <Utils/Fsm/actions/Update.h>

#include <string>

#include "DialogEvents.h"

namespace GameEngine
{
class IdleState;
class TransitionState;
class WaitingForInput;
struct DialogContext;
struct DialogueNode;

class ShowingSentence : public Utils::StateMachine::Will<
                            Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                            Utils::StateMachine::On<EndDialog, Utils::StateMachine::TransitionTo<TransitionState>>,
                            Utils::StateMachine::On<StartInputWaiting, Utils::StateMachine::TransitionTo<WaitingForInput>>,
                            Utils::StateMachine::On<BackToSentence, Utils::StateMachine::TransitionTo<WaitingForInput>>,
                            Utils::StateMachine::On<StartSentence, Utils::StateMachine::Update>>
{
public:
    ShowingSentence(DialogContext&);

    void onEnter();
    void onEnter(const StartSentence&);
    void onEnter(const OptionSelected&);

    void update(const StartSentence&);

private:
    void createGuiTexts(const std::string&, const std::string&);

private:
    DialogContext& dialogContext;
};
}  // namespace GameEngine
