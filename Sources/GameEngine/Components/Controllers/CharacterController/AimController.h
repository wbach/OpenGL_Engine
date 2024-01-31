#pragma once
#include <Types.h>

namespace Input
{
class InputManager;
}
namespace GameEngine
{
namespace Animation
{
struct Joint;
} // namespace Animation
namespace Components
{
class AimController
{
public:
    AimController(Input::InputManager&, Animation::Joint&);
    void enter();
    void update();
    void reset();
    const Animation::Joint& getJoint() const;

private:
    vec2 calculateMouseMove();
    void LockPitch();
    void LockYaw();

private:
    Input::InputManager& inputManager;
    Animation::Joint& joint;
    float pitch;
    float yaw;
    float camSensitive;
    vec2 yawLimit;
    vec2 pitchLimit;
};
}  // namespace Components
}  // namespace GameEngine
