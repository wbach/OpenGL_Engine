#include "JointPoseUpdater.h"

#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string COMPONENT_STR{"JointPoseUpdater"};
const std::string CSTR_WEAPON_JOINT_NAME = "jointName";
}  // namespace
JointPoseUpdater::JointPoseUpdater(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(JointPoseUpdater).hash_code(), componentContext, gameObject)
{
}
void JointPoseUpdater::CleanUp()
{
}
void JointPoseUpdater::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, [this]() {
        auto parent = thisObject_.GetParent();

        if (not parent)
        {
            WARNING_LOG("Parent not found.");
            return;
        }

        auto animator = parent->GetComponent<Animator>();
        if (not animator)
        {
            WARNING_LOG("Animator not found");
            return;
        }
        joint_ = animator->GetJoint(jointName_);

        if (not joint_)
        {
            WARNING_LOG("Joint: \"" + jointName_ + "\" not found");
            return;
        }

        auto rendererCopmponent = parent->GetComponent<RendererComponent>();

        if (not rendererCopmponent)
        {
            WARNING_LOG("RendererComponent not found");
            return;
        }

        auto model = rendererCopmponent->GetModelWrapper().Get();

        if (not model or model->GetMeshes().empty())
        {
            WARNING_LOG("Mesh not found");
            return;
        }

        meshTransform_ = model->GetMeshes().front().GetMeshTransform();
        calculateOffsets();
    });
}
void JointPoseUpdater::updateGameObjectTransform()
{
    auto parent = thisObject_.GetParent();

    if (not meshTransform_ or not joint_ or not parent)
    {
        return;
    }

    // path bone to world pos
    // worldPosition = parentMatrix * parentMeshMatrix * jointMatrix * inverse(jointOffset) * point

    auto worldBoneMatrix = parent->GetWorldTransform().GetMatrix() * (*meshTransform_) * joint_->animatedTransform *
                           glm::inverse(joint_->offset);
    worldBoneMatrix                                    = worldBoneMatrix * glm::translate(positionOffset_);
    auto [boneWorldPosition, boneWorldRotation, _] = Utils::decompose(worldBoneMatrix);

    thisObject_.SetWorldPositionRotation(boneWorldPosition, boneWorldRotation * rotationOffset_.value_);
}
void JointPoseUpdater::calculateOffsets()
{
    auto worldBoneMatrix = thisObject_.GetParent()->GetWorldTransform().GetMatrix() * (*meshTransform_) * glm::inverse(joint_->offset);
    auto [boneWorldPosition, boneWorldRotation, _] = Utils::decompose(worldBoneMatrix);

    const auto& position = thisObject_.GetWorldTransform().GetPosition();
    const auto& rotation = thisObject_.GetWorldTransform().GetRotation();

    positionOffset_ = position - boneWorldPosition;
    rotationOffset_ = rotation.value_ - boneWorldRotation;
}
void JointPoseUpdater::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<JointPoseUpdater>(componentContext, gameObject);
        ::Read(node.getChild(CSTR_WEAPON_JOINT_NAME), component->jointName_);
        return component;
    };

    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}
void JointPoseUpdater::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
    node.addChild(CSTR_WEAPON_JOINT_NAME, jointName_);
}
}  // namespace Components
}  // namespace GameEngine
