#pragma once
#include <Common/Controllers/CharacterController/Character.h>
#include <Common/Controllers/CharacterController/CharacterActions.h>
#include <GameEngine/Components/Physics/Rigidbody.h>

#include "CharacterStatistic.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "Bar.h"

namespace GameEngine
{
class GuiManager;
class GuiTextureElement;
class GuiWindowElement;

namespace Components
{
class Player : public BaseComponent
{
    struct HudElements
    {
        Bar hp;
        Bar stamina;
        Bar mana;
        GuiWindowElement* window{nullptr};
    };

public:
    Player(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;

    void Init();
    void Update();
    void hurt(int64);
    void renderDmg(const common::Transform&, int64);

private:
    GuiManager& guiManager_;
    Animator* animator_;
    CharacterController* characterController_;
    CharacterStatistic characterStatistic_;
    HudElements hudElements_;
    std::vector<IdType> animSubs_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
