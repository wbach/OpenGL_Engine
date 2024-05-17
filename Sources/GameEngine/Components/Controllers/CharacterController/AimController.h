#pragma once
#include <Types.h>
#include <Utils/IdPool.h>
#include <memory>

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
class Animator;
class Rigidbody;
class ThridPersonCameraComponent;
class JointPoseUpdater;

class AimController
{
public:
    AimController(Scene&, GameObject&, Input::InputManager&, Animation::Joint&);
    ~AimController();
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
    Animator* animator;
    float camSensitive;
    vec2 boneRotatationLimit;

    std::unique_ptr<Components::JointPoseUpdater> jointPoseUpdater;
    std::optional<uint32> updateJointBufferSubId_;
    std::optional<IdType> subscribeForDrawArrowAnimationFrame;
};
}  // namespace Components
}  // namespace GameEngine
