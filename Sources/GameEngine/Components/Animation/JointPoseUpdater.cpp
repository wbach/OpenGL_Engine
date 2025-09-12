#include "JointPoseUpdater.h"

#include <Logger/Log.h>
#include <Utils/TreeNode.h>

#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
JointPoseUpdater::JointPoseUpdater(GameObject& go, Animation::Joint* joint, const mat4& meshTransform)
    : owner(go)
    , joint{joint}
    , meshTransform{meshTransform}
{
}

JointPoseUpdater::JointPoseUpdater(GameObject& go, Animation::Joint* j, const vec3& localPosition,
                                   const Rotation& localRotation, const mat4& meshTransform)
    : owner(go)
    , localPosition{localPosition}
    , localRotation{localRotation}
    , joint{j}
    , meshTransform{meshTransform}
{
    fillOffsets();
}

void JointPoseUpdater::fillOffsets()
{
    auto parent = owner.GetParent();
    if (not joint or not parent)
    {
        LOG_ERROR << "Joint or parent not set!";
        return;
    }

    auto currentParentWorldMatrix = parent->GetWorldTransform().CalculateCurrentMatrix();

    auto worldBoneMatrix = currentParentWorldMatrix * meshTransform * glm::inverse(joint->offset);
    auto [boneWorldPosition, boneWorldRotation, _] = Utils::decompose(worldBoneMatrix);

    owner.GetTransform().SetPositionAndRotation(localPosition, localRotation);

    const auto& position = owner.GetWorldTransform().GetPosition();
    const auto& rotation = owner.GetWorldTransform().GetRotation();

    offset.position = position - boneWorldPosition;
    //  joint.offset.rotation = rotation.value_ - boneWorldRotation;
    offset.rotation = rotation.value_ * glm::inverse(boneWorldRotation);
}

void JointPoseUpdater::updateGameObjectTransform()
{
    auto parent = owner.GetParent();

    if (not parent)
    {
        return;
    }

    // path bone to world pos
    // worldPosition = parentMatrix * parentMeshMatrix * jointMatrix * inverse(jointOffset) * point

    mat4 localMatrix(1.0);
    localMatrix *= glm::translate(offset.position);
    localMatrix *= glm::mat4_cast(offset.rotation.value_);

    auto boneMatrix = parent->GetWorldTransform().CalculateCurrentMatrix() * meshTransform * joint->animatedTransform *
                      glm::inverse(joint->offset);

    // bone as new Parent for weapon
    auto [bonePosition, boneRotation, scale] = Utils::decompose(boneMatrix);

    mat4 boneWorldMatrix(1.0);
    boneWorldMatrix *= glm::translate(bonePosition);
    boneWorldMatrix *= glm::mat4_cast(boneRotation);

    auto [newPosition, newRotation, _] = Utils::decompose(boneWorldMatrix * localMatrix);
    owner.SetWorldPositionRotation(newPosition, newRotation);
}
}  // namespace Components
}  // namespace GameEngine
