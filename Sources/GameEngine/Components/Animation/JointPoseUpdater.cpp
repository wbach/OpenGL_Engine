#include "JointPoseUpdater.h"

#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/SceneWriter.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string COMPONENT_STR{"JointPoseUpdater"};
const std::string CSTR_WEAPON_EQUIP_JOINT_NAME  = "equipJointName";
const std::string CSTR_WEAPON_DISARM_JOINT_NAME = "disarmJointName";

// offset are calucalted based on reference position in rotation, example usage: weapon which is child of weapon owner
// could be set in editor in right place and this values can treat as reference

const std::string CSTR_OFFSET_DISARM_REFERENCE = "offsetDisarmReference";
const std::string CSTR_OFFSET_EQUIP_REFERENCE  = "offsetEquipReference";

}  // namespace
JointPoseUpdater::JointPoseUpdater(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(JointPoseUpdater).hash_code(), componentContext, gameObject)
    , currentJoint_{nullptr}
{
}
void JointPoseUpdater::CleanUp()
{
    if (updateJointBufferSubId_)
    {
        auto parent   = thisObject_.GetParent();
        auto animator = parent->GetComponent<Animator>();
        animator->unSubscribeForPoseUpdateBuffer(*updateJointBufferSubId_);
    }
}
void JointPoseUpdater::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart,
                     [this]()
                     {
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
                         equipJoint_.joint = animator->GetJoint(equipJointName_);

                         if (not equipJoint_.joint)
                         {
                             WARNING_LOG("equip joint: \"" + equipJointName_ + "\" not found");
                             return;
                         }

                         disarmJoint_.joint = animator->GetJoint(disarmJointName_);

                         if (not disarmJoint_.joint)
                         {
                             WARNING_LOG("disam joint: \"" + disarmJointName_ + "\" not found");
                             return;
                         }

                         currentJoint_ = &disarmJoint_;

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
                         updateJointBufferSubId_ =
                             animator->subscribeForPoseBufferUpdate([this]() { updateGameObjectTransform(); });

                         fillOffsetsForJoint(equipJoint_);
                         fillOffsetsForJoint(disarmJoint_);
                     });
}
void JointPoseUpdater::updateGameObjectTransform()
{
    auto parent = thisObject_.GetParent();

    if (not meshTransform_ or not currentJoint_ or not parent)
    {
        return;
    }

    // path bone to world pos
    // worldPosition = parentMatrix * parentMeshMatrix * jointMatrix * inverse(jointOffset) * point

    mat4 localMatrix(1.0);
    localMatrix *= glm::translate(currentJoint_->offset.position);
    localMatrix *= glm::mat4_cast(currentJoint_->offset.rotation.value_);

    auto boneMatrix = parent->GetWorldTransform().CalculateCurrentMatrix() * (*meshTransform_) *
                      currentJoint_->joint->animatedTransform * glm::inverse(currentJoint_->joint->offset);

    // bone as new Parent for weapon
    auto [bonePosition, boneRotation, scale] = Utils::decompose(boneMatrix);

    mat4 boneWorldMatrix(1.0);
    boneWorldMatrix *= glm::translate(bonePosition);
    boneWorldMatrix *= glm::mat4_cast(boneRotation);

    auto [newPosition, newRotation, _] = Utils::decompose(boneWorldMatrix * localMatrix);
    thisObject_.SetWorldPositionRotation(newPosition, newRotation);
}

void JointPoseUpdater::setEquipJointAsCurrent()
{
    currentJoint_ = &equipJoint_;
}

void JointPoseUpdater::setDisarmJointAsCurrent()
{
    currentJoint_ = &disarmJoint_;
}

void JointPoseUpdater::fillOffsetsForJoint(Joint& joint)
{
    auto currentParentWorldMatrix = thisObject_.GetParent()->GetWorldTransform().CalculateCurrentMatrix();

    auto worldBoneMatrix = currentParentWorldMatrix * (*meshTransform_) * glm::inverse(joint.joint->offset);
    auto [boneWorldPosition, boneWorldRotation, _] = Utils::decompose(worldBoneMatrix);

    thisObject_.GetTransform().SetPositionAndRotation(joint.localPosition, joint.localRotation);

    const auto& position = thisObject_.GetWorldTransform().GetPosition();
    const auto& rotation = thisObject_.GetWorldTransform().GetRotation();

    joint.offset.position = position - boneWorldPosition;
    //  joint.offset.rotation = rotation.value_ - boneWorldRotation;
    joint.offset.rotation = rotation.value_ * glm::inverse(boneWorldRotation);
}

void JointPoseUpdater::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        vec3 eulersRotation;

        auto component = std::make_unique<JointPoseUpdater>(componentContext, gameObject);
        ::Read(node.getChild(CSTR_WEAPON_EQUIP_JOINT_NAME), component->equipJointName_);
        ::Read(node.getChild(CSTR_WEAPON_DISARM_JOINT_NAME), component->disarmJointName_);

        auto disarmOffsetNode = node.getChild(CSTR_OFFSET_DISARM_REFERENCE);
        ::Read(disarmOffsetNode->getChild(CSTR_POSITION), component->disarmJoint_.localPosition);
        ::Read(disarmOffsetNode->getChild(CSTR_ROTATION), eulersRotation);
        component->disarmJoint_.localRotation = Rotation(DegreesVec3(eulersRotation));

        auto equpOffsetNode = node.getChild(CSTR_OFFSET_EQUIP_REFERENCE);
        ::Read(equpOffsetNode->getChild(CSTR_POSITION), component->equipJoint_.localPosition);
        ::Read(equpOffsetNode->getChild(CSTR_ROTATION), eulersRotation);
        component->equipJoint_.localRotation = Rotation(DegreesVec3(eulersRotation));
        return component;
    };

    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}

void JointPoseUpdater::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
    node.addChild(CSTR_WEAPON_EQUIP_JOINT_NAME, equipJointName_);
    node.addChild(CSTR_WEAPON_DISARM_JOINT_NAME, disarmJointName_);
    auto& disarmNode = node.addChild(CSTR_OFFSET_DISARM_REFERENCE);
    ::write(disarmNode.addChild(CSTR_POSITION), disarmJoint_.localPosition);
    ::write(disarmNode.addChild(CSTR_ROTATION), disarmJoint_.localRotation.GetEulerDegrees().value);

    auto& equipNode = node.addChild(CSTR_OFFSET_EQUIP_REFERENCE);
    ::write(equipNode.addChild(CSTR_POSITION), equipJoint_.localPosition);
    ::write(equipNode.addChild(CSTR_ROTATION), equipJoint_.localRotation.GetEulerDegrees().value);
}
}  // namespace Components
}  // namespace GameEngine
