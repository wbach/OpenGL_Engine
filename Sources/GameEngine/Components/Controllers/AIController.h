#pragma once
#include "GameEngine/Components/Component.h"

namespace GameEngine
{
namespace Components
{
class CharacterController;

DECLARE_COMPONENT(AIController)
{
public:
    AIController(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void Init();
    void Update();

    void MoveTo(const vec3&);

    std::vector<vec3> currentPath_;

private:
    void calculateMovingPoints();
    void UpdateNavigation();

private:
    CharacterController* characterController_;
    bool isMovingForward_{false};

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
