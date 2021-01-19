#pragma once
#include <unordered_map>

#include "Common.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/ShaderBuffers/PerPoseUpdate.h"
#include "PlayDirection.h"
#include "StateMachine.h"

namespace common
{
class Trasnform;
}  // namespace common

namespace GameEngine
{
class Mesh;

namespace Components
{
class RendererComponent;

struct JointData
{
    JointData(GraphicsApi::IGraphicsApi& api)
        : api_(api)
    {
    }
    Pose pose;
    Animation::Joint rootJoint;
    std::unique_ptr<BufferObject<PerPoseUpdate>> buffer;

    void updateBufferTransform();
    void updateBufferTransform(Animation::Joint&);

    GraphicsApi::IGraphicsApi& api_;
};

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

protected:
    void updateShaderBuffers();
    bool IsReady();
    void GetSkeletonAndAnimations();
    void applyPoseToJoints(const Pose&, Animation::Joint&, const mat4&);
    void applyPoseToJoints(const Pose&);
    void updateConnectedObjectToJoint(uint32, const Animation::Joint&);

protected:
    StateMachine machine_;

    RendererComponent* rendererComponent_;
    std::string requestedAnimationToset_;
    std::unordered_map<uint32, ConnectedObject> connectedObjects_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
