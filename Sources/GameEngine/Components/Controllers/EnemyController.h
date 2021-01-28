#pragma once
#include <Common/Controllers/CharacterController/Character.h>
#include <Common/Controllers/CharacterController/CharacterActions.h>
#include <GameEngine/Components/Physics/Rigidbody.h>

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/Controllers/CharacterController.h"

namespace GameEngine
{
namespace Components
{
class Enemy;
class EnemyController : public BaseComponent
{
public:
    EnemyController(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;

    void Init();
    void Update();

private:
    Quaternion caclulateTargetRotation(const vec3&) const;
    void clearStates();

private:
    Enemy* enemy_;
    Animator* animator_;
    CharacterController* characterController_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
