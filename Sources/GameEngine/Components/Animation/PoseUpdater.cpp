#include "PoseUpdater.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/SceneWriter.h"
#include "JointPoseUpdater.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string CSTR_WEAPON_EQUIP_JOINT_NAME  = "equipJointName";
const std::string CSTR_WEAPON_DISARM_JOINT_NAME = "disarmJointName";

// offset are calucalted based on reference position in rotation, example usage: weapon which is child of weapon owner
// could be set in editor in right place and this values can treat as reference

const std::string CSTR_OFFSET_DISARM_REFERENCE = "offsetDisarmReference";
const std::string CSTR_OFFSET_EQUIP_REFERENCE  = "offsetEquipReference";

}  // namespace

REGISTER_COMPONENT(PoseUpdater)

PoseUpdater::PoseUpdater(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
    , currentJointUpdater_{nullptr}
{
}
PoseUpdater::~PoseUpdater()
{
}
void PoseUpdater::CleanUp()
{
    if (updateJointBufferSubId_)
    {
        auto parent   = thisObject_.GetParent();
        auto animator = parent->GetComponent<Animator>();
        animator->unSubscribeForPoseUpdateBuffer(*updateJointBufferSubId_);
        updateJointBufferSubId_.reset();
    }

    disarmJointUpdater_.reset();
    equipJointUpdater_.reset();
    currentJointUpdater_ = nullptr;
}
void PoseUpdater::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, [this]() { Init(); });
}

void PoseUpdater::Reload()
{
    LOG_DEBUG << "Reload";

    bool isEquipState = (currentJointUpdater_ == equipJointUpdater_.get());

    CleanUp();
    Init();

    if (isEquipState)
    {
        setEquipJointAsCurrent();
    }
}

void PoseUpdater::Init()
{
    auto parent = thisObject_.GetParent();

    if (not parent)
    {
        LOG_WARN << "Parent not found.";
        return;
    }

    auto animator = parent->GetComponent<Animator>();
    if (not animator)
    {
        LOG_WARN << "Animator not found";
        return;
    }
    auto equipJoint = animator->GetJoint(equipJointName_);

    if (not equipJoint)
    {
        LOG_WARN << "equip joint: \"" << equipJointName_ << "\" not found";
        return;
    }

    auto disarmJoint = animator->GetJoint(disarmJointName_);

    if (not disarmJoint)
    {
        LOG_WARN << "disam joint: \"" << disarmJointName_ << "\" not found";
        return;
    }

    auto rendererCopmponent = parent->GetComponent<RendererComponent>();

    if (not rendererCopmponent)
    {
        LOG_WARN << "RendererComponent not found";
        return;
    }

    auto model = rendererCopmponent->GetModelWrapper().Get();

    if (not model or model->GetMeshes().empty())
    {
        LOG_WARN << "Mesh not found";
        return;
    }

    dLocalRotation = Rotation(DegreesVec3(dLocalRotationEuler));
    eLocalRotation = Rotation(DegreesVec3(eLocalRotationEuler));

    auto meshTransform = model->GetMeshes().front().GetMeshTransform();
    updateJointBufferSubId_ =
        animator->subscribeForPoseBufferUpdate([this]() { currentJointUpdater_->updateGameObjectTransform(); });

    disarmJointUpdater_ =
        std::make_unique<JointPoseUpdater>(thisObject_, disarmJoint, dLocalPosition, dLocalRotation, meshTransform);
    equipJointUpdater_ =
        std::make_unique<JointPoseUpdater>(thisObject_, equipJoint, eLocalPosition, eLocalRotation, meshTransform);

    setDisarmJointAsCurrent();
}

void PoseUpdater::setEquipJointAsCurrent()
{
    currentJointUpdater_ = equipJointUpdater_.get();
}

void PoseUpdater::setDisarmJointAsCurrent()
{
    currentJointUpdater_ = disarmJointUpdater_.get();
}

void PoseUpdater::read(const TreeNode& node)
{
    ::Read(node.getChild(CSTR_WEAPON_EQUIP_JOINT_NAME), equipJointName_);
    ::Read(node.getChild(CSTR_WEAPON_DISARM_JOINT_NAME), disarmJointName_);

    if (auto disarmOffsetNode = node.getChild(CSTR_OFFSET_DISARM_REFERENCE))
    {
        ::Read(disarmOffsetNode->getChild(CSTR_POSITION), dLocalPosition);
        ::Read(disarmOffsetNode->getChild(CSTR_ROTATION), dLocalRotationEuler);
    }
    if (auto equpOffsetNode = node.getChild(CSTR_OFFSET_EQUIP_REFERENCE))
    {
        ::Read(equpOffsetNode->getChild(CSTR_POSITION), eLocalPosition);
        ::Read(equpOffsetNode->getChild(CSTR_ROTATION), eLocalRotationEuler);
    }
}
void PoseUpdater::write(TreeNode& node) const
{
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
