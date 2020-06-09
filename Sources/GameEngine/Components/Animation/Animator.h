#pragma once
#include <unordered_map>

#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/ShaderBuffers/PerPoseUpdate.h"
#include "GameEngine/Resources/BufferObject.h"

namespace GameEngine
{
class Mesh;

namespace Components
{
typedef std::unordered_map<std::string, mat4> Pose;

struct JointData
{
    Animation::Joint rootJoint;
    std::unique_ptr<BufferObject<PerPoseUpdate>> buffer;

    void updateBufferTransform();
    void updateBufferTransform(Animation::Joint&);
};

class Animator : public BaseComponent
{
public:
    Animator(ComponentContext&, GameObject&);
    void CleanUp() override;
    void ReqisterFunctions() override;

    void Update();
    Animator& SetAnimation(const std::string&);
    void ChangeAnimation(const std::string&);
    const std::string& GetCurrentAnimationName() const;
    const GraphicsApi::ID& getPerPoseBufferId(uint32) const;

public:
    std::unordered_map<std::string, Animation::AnimationClip> animationClips_;
    std::vector<JointData> meshRootJoints_;
    float currentTime_;
    float animationSpeed_;
    float changeAnimTime_;

protected:
    void updateShaderBuffers();
    void ChangeAnimState();
    bool IsReady();
    void increaseAnimationTime();
    void GetSkeletonAndAnimations();
    std::pair<Animation::KeyFrame, Animation::KeyFrame> getPreviousAndNextFrames();
    float calculateProgression(const Animation::KeyFrame& previousFrame, const Animation::KeyFrame& nextFrame);
    Pose calculateCurrentAnimationPose();
    Pose interpolatePoses(const Animation::KeyFrame& previousFrame, const Animation::KeyFrame& nextFrame,
                          float progression);
    void applyPoseToJoints(const Pose&, Animation::Joint&, const mat4&);
    void applyPoseToJoints(const Pose&);

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
