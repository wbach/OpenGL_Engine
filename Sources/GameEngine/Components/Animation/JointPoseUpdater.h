#pragma once
#include <unordered_map>

#include "GameEngine/Components/BaseComponent.h"
#include "JointData.h"

namespace GameEngine
{
namespace Components
{
class JointPoseUpdater : public BaseComponent
{
public:
    JointPoseUpdater(ComponentContext&, GameObject&);
    void CleanUp() override;
    void ReqisterFunctions() override;

    void updateGameObjectTransform();  // Call on rendering thread
    void setEquipJointAsCurrent();
    void setDisarmJointAsCurrent();

private:
    struct Offset
    {
        vec3 position{0.f};
        Rotation rotation{RadiansVec3(0.f)};

    };
    struct Joint
    {
        Offset offset;
        vec3 localPosition{0.f};
        Rotation localRotation{RadiansVec3(0.f)};
        Animation::Joint* joint{nullptr};
    };

    void fillOffsetsForJoint(Joint&);

public:
    std::string equipJointName_;
    std::string disarmJointName_;

private:
    Joint equipJoint_;
    Joint disarmJoint_;
    Joint* currentJoint_;

    std::optional<mat4> meshTransform_;
    std::optional<uint32> updateJointBufferSubId_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
