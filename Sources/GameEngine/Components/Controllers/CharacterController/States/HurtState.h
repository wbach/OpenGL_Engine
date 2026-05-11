#pragma once
#include <optional>

#include "CharacterControllerCommonDefs.h"
#include "Fsm/actions/BackToPreviousState.h"
#include "GameEngine/Components/Controllers/CharacterController/States/BaseState.h"
#include "IdleStateBase.h"
#include "Types.h"

namespace GameEngine
{
namespace Components
{
class HurtState
    : public BaseState
    , public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                                       Utils::StateMachine::On<AnimationPlayEndEvent, Utils::StateMachine::BackToPreviousState>,
                                       Utils::StateMachine::On<DeathEvent, Utils::StateMachine::Queue>>
{
public:
    HurtState(FsmContext&);

    bool entryCondition() const;
    void onEnter();
    void onLeave();

private:
    std::optional<IdType> animationSubId;
};
}  // namespace Components
}  // namespace GameEngine
