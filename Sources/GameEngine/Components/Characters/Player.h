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
class Player : public BaseComponent
{
public:
    Player(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;

    void Init();
    void Update();

private:
    void isOnGround();

private:
    Animator* animator_;
    CharacterController* characterController_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
