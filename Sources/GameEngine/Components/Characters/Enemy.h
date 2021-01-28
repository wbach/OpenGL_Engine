#pragma once
#include <Common/Controllers/CharacterController/Character.h>
#include <GameEngine/Components/Physics/Rigidbody.h>
#include "GameEngine/Components/BaseComponent.h"

#include <Common/Controllers/CharacterController/CharacterActions.h>
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Controllers/CharacterController.h"
#include "CharacterStatistic.h"

namespace GameEngine
{
namespace Components
{
class Enemy : public BaseComponent
{
public:
    Enemy(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;

    void Init();
    void Update();
    void hurt(int64);
    const CharacterStatistic& characterStatistic() const;

private:
    void isOnGround();

private:
    Animator* animator_;
    CharacterController* characterController_;
    CharacterStatistic characterStatistic_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
