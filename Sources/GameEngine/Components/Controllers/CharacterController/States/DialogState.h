#pragma once
#include "CharacterControllerCommonDefs.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Dialogs/Fsm/DialogEvents.h"
#include "IdleStateBase.h"

namespace GameEngine
{
namespace Components
{
class DialogState : public IdleStateBase,
                    public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                                                     Utils::StateMachine::On<StartDialogEvent, Utils::StateMachine::Update>,
                                                     Utils::StateMachine::On<EndDialogEvent, Utils::StateMachine::BackToPreviousState>>
{
public:
    DialogState(FsmContext &);

    void onEnter(const StartDialogEvent &);

    using BaseState::update;

    void update(const StartDialogEvent &);
};
}  // namespace Components
}  // namespace GameEngine
