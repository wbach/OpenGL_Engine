#pragma once
#include <Input/KeysSubscriptionsManager.h>
#include <Utils/Fsm/Actions.h>
#include <Utils/Fsm/actions/Update.h>

#include <cstddef>

#include "DialogEvents.h"

namespace GameEngine
{
class IdleState;
class TransitionState;
class ShowingSentence;
struct DialogContext;
class GameObject;

class WaitingForInput : public Utils::StateMachine::Will<
                            Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                            Utils::StateMachine::On<EndDialog, Utils::StateMachine::TransitionTo<TransitionState>>,
                            Utils::StateMachine::On<StartInputWaiting, Utils::StateMachine::Update>,
                            Utils::StateMachine::On<OptionSelected, Utils::StateMachine::TransitionTo<ShowingSentence>>>
{
public:
    WaitingForInput(DialogContext&);

    void onEnter();
    void onEnter(const StartInputWaiting&);
    void onEnter(const BackToSentence&);
    void onLeave();

private:
    void subscribeForInput();
    void updateHighLightedColor(size_t, size_t);
    void selectOption(size_t);
    void setAnimations(GameObject&, GameObject&);

private:
    DialogContext& dialogContext;
    GameObject* playerGameObject{nullptr};
    Components::DialogueComponent* component{nullptr};
    Input::KeysSubscriptionsManager subscriptions_;
    std::vector<std::pair<int, DialogueOption>> visibleOptions;
    size_t highlighted{0};
};
}  // namespace GameEngine
