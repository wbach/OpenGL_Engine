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

    template <typename Event>
    void pushEventToQueue(const Event &event)
    {
        DEBUG_LOG("pushEventToQueue: " + typeName<Event>());
        queue.push_back(event);
    }
    template <typename Event>
    void removeEventFromQueue(const Event &event)
    {
        DEBUG_LOG("removeEventFromQueue: " + typeName<Event>());

        queue.erase(
            std::remove_if(queue.begin(), queue.end(), [](const auto &event) { return std::holds_alternative<Event>(event); }));
    }

private:
    std::vector<CharacterControllerEvent> queue;
    std::string animName;
};
}  // namespace Components
}  // namespace GameEngine
