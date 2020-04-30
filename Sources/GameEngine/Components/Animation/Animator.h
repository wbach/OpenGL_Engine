#pragma once
#include <unordered_map>
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
class Mesh;

namespace Components
{
typedef std::unordered_map<std::string, mat4> Pose;

class Animator : public BaseComponent
{
public:
    Animator(ComponentContext&, GameObject&);
    void CleanUp() override;
    void ReqisterFunctions() override;

    void Update();
    Animator& SetSkeleton(Animation::Joint* skeleton);
    Animator& SetAnimation(const std::string& name);
    void ChangeAnimation(const std::string& name);
    const std::string& GetCurrentAnimationName() const;

public:
    std::unordered_map<std::string, Animation::AnimationClip> animationClips_;
    Animation::Joint* rootJoint_;
    float currentTime_;
    float animationSpeed_;
    float changeAnimTime_;

protected:
    void ChangeAnimState();
    bool IsReady();
    void increaseAnimationTime();
    void GetSkeletonAndAnimations();
    std::pair<Animation::KeyFrame, Animation::KeyFrame> getPreviousAndNextFrames();
    float calculateProgression(const Animation::KeyFrame& previousFrame, const Animation::KeyFrame& nextFrame);
    Pose calculateCurrentAnimationPose();
    Pose interpolatePoses(const Animation::KeyFrame& previousFrame, const Animation::KeyFrame& nextFrame,
                          float progression);
    void applyPoseToJoints(const Pose& currentPose, Animation::Joint& joint, const mat4& parentTransform);

protected:
    uint32 currentFrameId_ = 0;
    std::string current_;

    std::string nextClip_;
    bool changeAnim              = false;
    float currentChangeAnimTime_ = 0;
    Animation::KeyFrame startChaneAnimPose;
    Animation::KeyFrame endChangeAnimPose;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
