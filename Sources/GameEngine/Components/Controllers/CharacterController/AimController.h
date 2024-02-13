#pragma once
#include <Types.h>

namespace Input
{
class InputManager;
}
namespace GameEngine
{
class GameObject;

namespace Animation
{
struct Joint;
}  // namespace Animation
namespace Components
{
class Rigidbody;
class ThridPersonCameraComponent;

class AimController
{
public:
    AimController(GameObject&, Input::InputManager&, Animation::Joint&);
    void enter();
    void update();
    void reset();
    const Animation::Joint& getJoint() const;

private:
    vec2 calculateMouseMove();
    void LockPitch(float&);
    void LockYaw(float&);
    void rotateCharacter(float);

private:
    GameObject& gameObject;
    Input::InputManager& inputManager;
    Animation::Joint& joint;
    ThridPersonCameraComponent* thridPersonCameraComponent;
    Rigidbody* rigidbody;
    float camSensitive;
    vec2 yawLimit;
    vec2 pitchLimit;
};
}  // namespace Components
}  // namespace GameEngine
