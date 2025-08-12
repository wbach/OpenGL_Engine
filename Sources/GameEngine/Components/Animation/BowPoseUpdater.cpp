#include "BowPoseUpdater.h"

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
const std::string COMPONENT_STR{"BowPoseUpdater"};
const std::string CSTR_WEAPON_EQUIP_JOINT_NAME  = "equipJointName";
const std::string CSTR_WEAPON_DISARM_JOINT_NAME = "disarmJointName";

// offset are calucalted based on reference position in rotation, example usage: weapon which is child of weapon owner
// could be set in editor in right place and this values can treat as reference

const std::string CSTR_OFFSET_DISARM_REFERENCE = "offsetDisarmReference";
const std::string CSTR_OFFSET_EQUIP_REFERENCE  = "offsetEquipReference";

}  // namespace
BowPoseUpdater::BowPoseUpdater(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(BowPoseUpdater).hash_code(), componentContext, gameObject)
    , currentJointUpdater_{nullptr}
{
}
void BowPoseUpdater::CleanUp()
{
    if (updateJointBufferSubId_)
    {
        auto parent   = thisObject_.GetParent();
        auto animator = parent->GetComponent<Animator>();
        animator->unSubscribeForPoseUpdateBuffer(*updateJointBufferSubId_);
    }
}
void BowPoseUpdater::ReqisterFunctions()
{
    RegisterFunction(
        FunctionType::OnStart,
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
            auto equipJoint = animator->GetJoint(equipJointName_);

            if (not equipJoint)
            {
                WARNING_LOG("equip joint: \"" + equipJointName_ + "\" not found");
                return;
            }

            auto disarmJoint = animator->GetJoint(disarmJointName_);

            if (not disarmJoint)
            {
                WARNING_LOG("disam joint: \"" + disarmJointName_ + "\" not found");
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
            auto meshTransform = model->GetMeshes().front().GetMeshTransform();
            updateJointBufferSubId_ =
                animator->subscribeForPoseBufferUpdate([this]() { currentJointUpdater_->updateGameObjectTransform(); });

            disarmJointUpdater_ =
                std::make_unique<JointPoseUpdater>(thisObject_, disarmJoint, dLocalPosition, dLocalRotation, meshTransform);
            equipJointUpdater_ =
                std::make_unique<JointPoseUpdater>(thisObject_, equipJoint, eLocalPosition, eLocalRotation, meshTransform);
            currentJointUpdater_ = disarmJointUpdater_.get();
            setDisarmJointAsCurrent();
        });
}

void BowPoseUpdater::setEquipJointAsCurrent()
{
    currentJointUpdater_ = equipJointUpdater_.get();
}

void BowPoseUpdater::setDisarmJointAsCurrent()
{
    currentJointUpdater_ = disarmJointUpdater_.get();
}

void BowPoseUpdater::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        vec3 eulersRotation;

        auto component = std::make_unique<BowPoseUpdater>(componentContext, gameObject);
        ::Read(node.getChild(CSTR_WEAPON_EQUIP_JOINT_NAME), component->equipJointName_);
        ::Read(node.getChild(CSTR_WEAPON_DISARM_JOINT_NAME), component->disarmJointName_);

        if (auto disarmOffsetNode = node.getChild(CSTR_OFFSET_DISARM_REFERENCE))
        {
            ::Read(disarmOffsetNode->getChild(CSTR_POSITION), component->dLocalPosition);
            ::Read(disarmOffsetNode->getChild(CSTR_ROTATION), eulersRotation);
            component->dLocalRotation = Rotation(DegreesVec3(eulersRotation));
        }
        if (auto equpOffsetNode = node.getChild(CSTR_OFFSET_EQUIP_REFERENCE))
        {
            ::Read(equpOffsetNode->getChild(CSTR_POSITION), component->eLocalPosition);
            ::Read(equpOffsetNode->getChild(CSTR_ROTATION), eulersRotation);
            component->eLocalRotation = Rotation(DegreesVec3(eulersRotation));
        }
        return component;
    };

    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}

void BowPoseUpdater::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
    node.addChild(CSTR_WEAPON_EQUIP_JOINT_NAME, equipJointName_);
    node.addChild(CSTR_WEAPON_DISARM_JOINT_NAME, disarmJointName_);
    auto& disarmNode = node.addChild(CSTR_OFFSET_DISARM_REFERENCE);

    ::write(disarmNode.addChild(CSTR_POSITION), dLocalPosition);
    ::write(disarmNode.addChild(CSTR_ROTATION), dLocalRotation.GetEulerDegrees().value);

    auto& equipNode = node.addChild(CSTR_OFFSET_EQUIP_REFERENCE);
    ::write(equipNode.addChild(CSTR_POSITION), eLocalPosition);
    ::write(equipNode.addChild(CSTR_ROTATION), eLocalRotation.GetEulerDegrees().value);
}
}  // namespace Components
}  // namespace GameEngine
