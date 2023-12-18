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

    void updateGameObjectTransform();

private:
    void calculateOffsets();

public:
    std::string jointName_;
    vec3 positionOffset_;
    Rotation rotationOffset_;

private:
    Animation::Joint* joint_;
    std::optional<mat4> meshTransform_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
