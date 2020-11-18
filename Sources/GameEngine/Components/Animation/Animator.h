#pragma once
#include <unordered_map>

#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/ShaderBuffers/PerPoseUpdate.h"

namespace GameEngine
{
class Mesh;

namespace Components
{
typedef std::unordered_map<std::string, mat4> Pose;

struct JointData
{
    JointData(GraphicsApi::IGraphicsApi& api)
        : api_(api)
    {
    }
    Animation::Joint rootJoint;
    std::unique_ptr<BufferObject<PerPoseUpdate>> buffer;

    void updateBufferTransform();
    void updateBufferTransform(Animation::Joint&);

    GraphicsApi::IGraphicsApi& api_;
};

class Animator : public BaseComponent
{
public:
    Animator(ComponentContext&, GameObject&);
    void CleanUp() override;
    void ReqisterFunctions() override;

    void Update();
    void AddAnimationClip(const GameEngine::File&);
    void AddAnimationClip(const Animation::AnimationClip&);
    Animator& SetAnimation(const std::string&);
    void ChangeAnimation(const std::string&);
    const std::string& GetCurrentAnimationName() const;
    GraphicsApi::ID getPerPoseBufferId() const;

public:
    std::unordered_map<std::string, Animation::AnimationClip> animationClips_;
    JointData jointData_;
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
    Animation::AnimationClip* currentAnimationClip_;
    Animation::AnimationClip* nextAnimationClip_;
	std::string requestedAnimationToset_;
    uint32 currentFrameId_ = 0;
    bool changeAnim              = false;
    float currentChangeAnimTime_ = 0;
    Animation::KeyFrame startChaneAnimPose;
    Animation::KeyFrame endChangeAnimPose;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
