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
class ThridPersonCameraComponent;

class AimController
{
public:
    AimController(Input::InputManager&, Animation::Joint&,  ThridPersonCameraComponent*);
    void enter();
    void update();
    void reset();
    const Animation::Joint& getJoint() const;

private:
    vec2 calculateMouseMove();
    void LockPitch(float&);
    void LockYaw(float&);

private:
    Input::InputManager& inputManager;
    Animation::Joint& joint;
    ThridPersonCameraComponent* thridPersonCameraComponent;
    float camSensitive;
    vec2 yawLimit;
    vec2 pitchLimit;
};
}  // namespace Components
}  // namespace GameEngine
