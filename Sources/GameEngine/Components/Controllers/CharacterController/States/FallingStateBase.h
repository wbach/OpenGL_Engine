#pragma once
#include "BaseState.h"
#include "CharacterControllerCommonDefs.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
class FallingStateBase : public BaseState
{
public:
    FallingStateBase(FsmContext &, const std::string &);

    void onEnter(const StartFallingEvent &);
    void update(float);
    void onLeave(const GroundDetectionEvent &);

    using StateAfterLand =
        Utils::StateMachine::Maybe<Utils::StateMachine::BackToPreviousState, Utils::StateMachine::TransitionTo<DisarmedIdleState>,
                                   Utils::StateMachine::TransitionTo<DeathState>>;

    StateAfterLand handle(const GroundDetectionEvent &);

private:
    std::string animName;
};
}  // namespace Components
}  // namespace GameEngine
