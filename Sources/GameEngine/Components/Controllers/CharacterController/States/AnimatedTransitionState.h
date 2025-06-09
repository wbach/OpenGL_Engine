#pragma once
#include <Utils/Variant.h>

#include <optional>
#include <vector>

#include "BaseState.h"
#include "CharacterControllerCommonDefs.h"

namespace GameEngine
{
namespace Components
{
class AnimatedTransitionState
    : public BaseState,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Queue>,
          Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<DisarmedFallingState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>>
{
public:
    AnimatedTransitionState(FsmContext&);

    void onEnter(DisarmedRunState&, DisarmedIdleState&);
    void onEnter(ArmedRunState&, ArmedIdleState&);

    using MaybeAttackStates = Utils::StateMachine::Maybe<Utils::StateMachine::TransitionTo<DisarmedIdleState>,
                                                         Utils::StateMachine::TransitionTo<ArmedIdleState>>;

    MaybeAttackStates handle(const AnimatedTransitionEndEvent&);

    using Will::handle;

private:
    void setAnimAndSubscribeForEnd(const std::string& animName);
    void setAnim(const std::string& animName);

private:
    std::optional<float> animSubId;
    std::variant<DisarmedIdleState*, ArmedIdleState*> targetStateVariant;
};
}  // namespace Components
}  // namespace GameEngine
