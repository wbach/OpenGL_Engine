#pragma once
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace Components
{
class CharacterController;

class AIController : public BaseComponent
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
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
