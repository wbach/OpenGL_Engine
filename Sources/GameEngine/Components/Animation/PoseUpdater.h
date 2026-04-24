#pragma once
#include <Rotation.h>

#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace Components
{
class JointPoseUpdater;
class PoseUpdater : public BaseComponent
{
public:
    vec3 dLocalPosition{0};
    vec3 eLocalPosition{0};
    vec3 dLocalRotationEuler{0};
    vec3 eLocalRotationEuler{0};

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_VECTOR3F(dLocalPosition)
        FIELD_VECTOR3F(eLocalPosition)
        FIELD_VECTOR3F(dLocalRotationEuler)
        FIELD_VECTOR3F(eLocalRotationEuler)
    END_FIELDS()
    // clang-format on

public:
    PoseUpdater(ComponentContext&, GameObject&);
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void setEquipJointAsCurrent();
    void setDisarmJointAsCurrent();

public:
    std::string equipJointName_;
    std::string disarmJointName_;

private:
    void Init();

    std::unique_ptr<JointPoseUpdater> equipJointUpdater_;
    std::unique_ptr<JointPoseUpdater> disarmJointUpdater_;
    JointPoseUpdater* currentJointUpdater_;

    std::optional<uint32> updateJointBufferSubId_;
    Rotation dLocalRotation, eLocalRotation;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
