#pragma once
#include <Utils/Fsm/Actions.h>

#include "../Context.h"
#include "../ThridPersonCameraEvents.h"

namespace GameEngine
{
namespace Animation
{
struct Joint;
}
namespace Components
{
namespace Camera
{
class TransitionState;
class AimState : public Utils::StateMachine::Will<
                     Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                     Utils::StateMachine::On<StopAimEvent, Utils::StateMachine::TransitionTo<TransitionState>>>
{
public:
    AimState(Context&, const vec3& = {-0.25f, 1.f, -0.75f});

    void onEnter(const StartAimEvent&);
    const vec4& getRelativeCamerePosition() const;
    const vec4& getLookAtPosition() const;

private:
    void cameraUpdate();
    void setJointIfNeeded(const StartAimEvent&);

private:
    Context& context;
    Animation::Joint* joint;

    vec4 referenceRelativeCamerePosition;
    vec4 relativeCamerePosition;
    vec4 lookAtLocalPosition;
    const mat4 yTranslation;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
