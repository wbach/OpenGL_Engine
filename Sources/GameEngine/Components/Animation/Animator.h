#pragma once
#include <unordered_map>
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Components/AbstractComponent.h"

namespace GameEngine
{
class Mesh;

namespace Components
{
typedef std::unordered_map<std::string, mat4> Pose;

class Animator : public AbstractComponent
{
public:
    Animator();
    void Update();
    void SetSkeleton(Animation::Joint* skeleton);
    virtual void ReqisterFunctions() override;
    void SetAnimation(const std::string& name);
    void ChangeAnimation(const std::string& name);
    const std::string& GetCurrentAnimationName();

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
}  // Components
}  // GameEngine
