#pragma once
#include <unordered_map>

#include "GameEngine/Components/BaseComponent.h"
#include "JointPoseUpdater.h"

namespace GameEngine
{
namespace Components
{
class BowPoseUpdater : public BaseComponent
{
public:
    BowPoseUpdater(ComponentContext&, GameObject&);
    void CleanUp() override;
    void ReqisterFunctions() override;

    void setEquipJointAsCurrent();
    void setDisarmJointAsCurrent();

public:
    std::string equipJointName_;
    std::string disarmJointName_;

private:
    std::unique_ptr<JointPoseUpdater> equipJointUpdater_;
    std::unique_ptr<JointPoseUpdater> disarmJointUpdater_;
    JointPoseUpdater* currentJointUpdater_;

    std::optional<uint32> updateJointBufferSubId_;

    vec3 dLocalPosition, eLocalPosition;
    Rotation dLocalRotation, eLocalRotation;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
