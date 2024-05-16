#pragma once
#include <Types.h>
#include <Utils/IdPool.h>

namespace Input
{
class InputManager;
}
namespace GameEngine
{
class GameObject;
class Scene;

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
    AimController(Scene&, GameObject&, Input::InputManager&, Animation::Joint&);
    void enter();
    void reload();
    void update();
    void reset();
    void shoot();
    const Animation::Joint& getJoint() const;

private:
    vec2 calculateMouseMove();
    void rotateCharacterIfBoneRotationExceeded(float&);
    void rotateCharacter(float);
    void createArrowObject();

private:
    Input::InputManager& inputManager;
    Animation::Joint& joint;
    Scene& scene;
    GameObject& gameObject;
    GameObject* arrowGameObject{nullptr};
    Utils::IdPool arrowIds;

    ThridPersonCameraComponent* thridPersonCameraComponent;
    Rigidbody* rigidbody;
    float camSensitive;
    vec2 boneRotatationLimit;
};
}  // namespace Components
}  // namespace GameEngine
