#pragma once
#include <Input/KeysSubscriptionsManager.h>
#include <Utils/Fsm/Actions.h>

#include "../Context.h"
#include "../ThridPersonCameraEvents.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
class AimState;

class RotateableRunState : public Utils::StateMachine::Will<
                               Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                               Utils::StateMachine::On<InitEvent, Utils::StateMachine::Update>,
                               Utils::StateMachine::On<StartAimEvent, Utils::StateMachine::TransitionTo<AimState>>>
{
public:
    RotateableRunState(Context&);
    ~RotateableRunState();

    void onEnter(const StopAimEvent&);
    void update(const InitEvent&);

private:
    void cameraUpdate();

private:
    Input::KeysSubscriptionsManager keysSubscriptionsManager;

    Context& context;

    float zoomSpeed_;
    vec4 relativeCamerePosition;

    std::optional<IdType> cameraId_;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
