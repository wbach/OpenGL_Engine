#include "Animator.h"
#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/Models/Mesh.h"

namespace GameEngine
{
using namespace Animation;

namespace Components
{
ComponentsType Animator::type = ComponentsType::Animator;

Animator::Animator(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::Animator, componentContext, gameObject)
    , rootJoint_(nullptr)
    , currentTime_(0.f)
    , animationSpeed_(1.f)
    , changeAnimTime_(0.25f)
    , current_("DefaultAnimationClip")
    , changeAnim(false)
{
}
void Animator::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Update, std::bind(&Animator::Update, this));
    RegisterFunction(FunctionType::Awake, std::bind(&Animator::GetSkeletonAndAnimations, this));
}
Animator& Animator::SetSkeleton(Animation::Joint* skeleton)
{
    rootJoint_ = skeleton;
    return *this;
}
Animator& Animator::SetAnimation(const std::string& name)
{
    current_     = name;
    currentTime_ = 0.f;
    return *this;
}
const std::string& Animator::GetCurrentAnimationName()
{
    return current_;
}
void Animator::ChangeAnimation(const std::string& name)
{
    auto& endFrames = animationClips_[name].GetFrames();
    if (endFrames.empty())
        return;

    endChangeAnimPose = endFrames[0];

    nextClip_              = name;
    changeAnim             = true;
    currentChangeAnimTime_ = 0;

    startChaneAnimPose = KeyFrame();
    for (const auto& p : calculateCurrentAnimationPose())
        startChaneAnimPose.transforms[p.first] = GetJointTransform(p.second);
}
void Animator::GetSkeletonAndAnimations()
{
    auto renderer = thisObject_.GetComponent<RendererComponent>();

    if (renderer == nullptr)
        return;

    auto model      = renderer->GetModelWrapper().Get(GameEngine::L1);
    rootJoint_      = &model->skeleton_;
    animationClips_ = model->animationClips_;

    if (!animationClips_.empty())
    {
        current_ = animationClips_.begin()->first;
    }
}
void Animator::ChangeAnimState()
{
    currentChangeAnimTime_ += (1.f / changeAnimTime_) * componentContext_.time_.deltaTime * animationSpeed_;

    if (currentChangeAnimTime_ > 1.f)
    {
        changeAnim             = false;
        current_               = nextClip_;
        currentTime_           = 0.f;
        currentChangeAnimTime_ = 1.f;
    }
    auto pos = interpolatePoses(startChaneAnimPose, endChangeAnimPose, currentChangeAnimTime_);
    applyPoseToJoints(pos, *rootJoint_, mat4());
}
bool Animator::IsReady()
{
    return (rootJoint_ != nullptr && !current_.empty() && animationClips_.count(current_) != 0);
}
void Animator::Update()
{
    if (!IsReady())
        return;

    if (changeAnim)
    {
        ChangeAnimState();
        return;
    }

    increaseAnimationTime();
    auto currentPose = calculateCurrentAnimationPose();
    applyPoseToJoints(currentPose, *rootJoint_, mat4());
}

void Animator::increaseAnimationTime()
{
    auto animationLength = animationClips_[current_].GetLength();

    currentTime_ += componentContext_.time_.deltaTime * animationSpeed_;
    if (currentTime_ > animationLength)
        currentTime_ = fmod(currentTime_, animationLength);
    if (currentTime_ < 0)
        currentTime_ = animationLength + currentTime_;
}

Pose Animator::calculateCurrentAnimationPose()
{
    auto frames       = getPreviousAndNextFrames();
    float progression = calculateProgression(frames.first, frames.second);
    return interpolatePoses(frames.first, frames.second, progression);
}

Pose Animator::interpolatePoses(const KeyFrame& previousFrame, const KeyFrame& nextFrame, float progression)
{
    Pose currentPose;
    for (const auto& pair : previousFrame.transforms)
    {
        auto& jointName = pair.first;
        if (previousFrame.transforms.count(jointName) == 0 || nextFrame.transforms.count(jointName) == 0)
            continue;

        JointTransform previousTransform = previousFrame.transforms.at(jointName);
        JointTransform nextTransform     = nextFrame.transforms.at(jointName);
        JointTransform currentTransform  = Interpolate(previousTransform, nextTransform, progression);
        currentPose[jointName]           = GetLocalTransform(currentTransform);
    }
    return currentPose;
}

std::pair<KeyFrame, KeyFrame> Animator::getPreviousAndNextFrames()
{
    const auto& allFrames = animationClips_[current_].GetFrames();

    KeyFrame previousFrame = allFrames[0];
    KeyFrame nextFrame     = allFrames[0];
    for (uint32 i = 1; i < allFrames.size(); i++)
    {
        nextFrame = allFrames[i];
        if (nextFrame.timeStamp > currentTime_)
            break;

        previousFrame = allFrames[i];
    }
    return {previousFrame, nextFrame};
}

void Animator::applyPoseToJoints(const Pose& currentPose, Joint& joint, const mat4& parentTransform)
{
    if (currentPose.count(joint.name) == 0)
        return;

    const auto& currentLocalTransform = currentPose.at(joint.name);
    auto currentTransform             = parentTransform * currentLocalTransform;
    joint.animatedTransform           = currentTransform * joint.invtransform;

    for (Joint& childJoint : joint.children)
    {
        applyPoseToJoints(currentPose, childJoint, currentTransform);
    }
}

float Animator::calculateProgression(const KeyFrame& previousFrame, const KeyFrame& nextFrame)
{
    float totalTime   = nextFrame.timeStamp - previousFrame.timeStamp;
    float currentTime = currentTime_ - previousFrame.timeStamp;
    return currentTime / totalTime;
}
}  // Animation
}  // GameEngine
