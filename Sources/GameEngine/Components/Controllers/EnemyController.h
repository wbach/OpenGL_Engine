#pragma once
#include <Common/Controllers/CharacterController/Character.h>
#include <GameEngine/Components/Physics/Rigidbody.h>
#include "GameEngine/Components/BaseComponent.h"

#include <Common/Controllers/CharacterController/CharacterActions.h>
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Controllers/CharacterController.h"

namespace GameEngine
{
namespace Components
{
class EnemyController : public BaseComponent
{
public:
    EnemyController(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;

    void Init();
    void Update();

    std::string idleAnimationName_;
    std::string runAnimationName_;
    std::string attackAnimationName_;

private:
    Animator* animator_;
    CharacterController* characterController_;


public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
