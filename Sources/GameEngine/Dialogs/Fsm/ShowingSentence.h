#pragma once
#include <Input/KeysSubscriptionsManager.h>
#include <Utils/Fsm/Actions.h>
#include <Utils/Fsm/actions/Update.h>

#include <optional>
#include <string>
#include <variant>

#include "DialogEvents.h"
#include "GameEngine/Audio/AudioId.h"
#include "GameEngine/Audio/PlayParameters.h"
#include "Types.h"

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
                            Utils::StateMachine::On<SkipRequested, Utils::StateMachine::Update>,
                            Utils::StateMachine::On<StartSentence, Utils::StateMachine::Update>>
{
public:
    ShowingSentence(DialogContext&);

    void onEnter();
    void onEnter(const StartSentence&);
    void onEnter(const OptionSelected&);

    void update(const StartSentence&);
    void update(const SkipRequested&);
    void onLeave();

private:
    void createGuiTexts(const std::string&, const std::string&);
    void subscribeForInput();

private:
    DialogContext& dialogContext;
    Input::KeysSubscriptionsManager subscriptions_;

    std::variant<AudioId, IdType, std::monostate> playId;
};
}  // namespace GameEngine
