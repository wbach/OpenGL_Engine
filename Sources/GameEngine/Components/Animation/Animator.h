#pragma once
#include <unordered_map>

#include "Common.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/ShaderBuffers/PerPoseUpdate.h"
#include "PlayDirection.h"
#include "StateMachine.h"
#include "JointData.h"

namespace common
{
class Trasnform;
}  // namespace common

namespace GameEngine
{
class Mesh;
class Model;

namespace Components
{
class RendererComponent;

class Animator : public BaseComponent
{
    struct ConnectedObject
    {
        GameObject& gameObject;
        vec3 worldPositionOffset;
        Rotation worldRotationOffset;
    };

public:
    enum class AnimationChangeType
    {
        smooth,
        direct
    };

    Animator(ComponentContext&, GameObject&);
    void CleanUp() override;
    void ReqisterFunctions() override;

    void Update();
    void AddAnimationClip(const GameEngine::File&);
    void AddAnimationClip(const Animation::AnimationClip&);
    Animator& SetAnimation(const std::string&);
    void ChangeAnimation(const std::string&, AnimationChangeType, PlayDirection = PlayDirection::forward);
    void MixAnimation(const std::vector<std::pair<std::string, std::string>>&);

    const std::string& GetCurrentAnimationName() const;
    GraphicsApi::ID getPerPoseBufferId() const;
    std::optional<uint32> connectBoneWithObject(const std::string&, GameObject&, const std::optional<vec3>& po = {},
                                                const std::optional<Rotation>& ro = {});
    void disconnectObjectFromBone(uint32);
    void setPlayOnceForAnimationClip(const std::string&);

public:
    std::unordered_map<std::string, Animation::AnimationClip> animationClips_;
    std::unordered_map<std::string, std::vector<std::function<void()>>> onAnimationEnd_;

    JointData jointData_;
    float animationSpeed_;
    std::string startupAnimationClipName_;

    std::unordered_map<std::string, std::vector<std::string>> jointGroups_;

protected:
    void updateShaderBuffers();
    void GetSkeletonAndAnimations();
    void applyPoseToJoints(Animation::Joint&, const mat4&);
    void applyPoseToJoints();
    void updateConnectedObjectToJoint(uint32, const Animation::Joint&);
    void createShaderJointBuffers();
    void initAnimationClips(const Model&);

protected:
    StateMachine machine_;

    RendererComponent* rendererComponent_;
    std::unordered_map<uint32, ConnectedObject> connectedObjects_;
    std::vector<GameEngine::File> clipsToRead_;
    std::unordered_map<std::string, std::vector<uint32>> jointGroupsIds_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
