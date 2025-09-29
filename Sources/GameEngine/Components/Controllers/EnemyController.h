#pragma once
#include <Common/Controllers/CharacterController/Character.h>
#include <Common/Controllers/CharacterController/CharacterActions.h>
#include <GameEngine/Components/Physics/Rigidbody.h>

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"

namespace GameEngine
{
namespace Components
{
class Enemy;
class EnemyController : public BaseComponent
{
    struct GeneratePathParams
    {
        bool isActive{false};
        float range{10.f};
        float offset{5.0f};
    };

public:
    EnemyController(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void Init();
    void Update();

private:
    Quaternion caclulateTargetRotation(const vec3&) const;
    void calculateMovingPoints();

private:
    Enemy* enemy_;
    Animator* animator_;
    CharacterController* characterController_;
    GeneratePathParams generatePathParams;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;

    vec3 freeWalkingTargetPoint;
    size_t freeWalkingTargetPointIndex;
    std::vector<vec3> movingPoints_;
};
}  // namespace Components
}  // namespace GameEngine
