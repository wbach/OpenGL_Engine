#pragma once
#include <Common/Controllers/CharacterController/Character.h>
#include <GameEngine/Components/Physics/Rigidbody.h>
#include "GameEngine/Components/BaseComponent.h"

#include <Common/Controllers/CharacterController/CharacterActions.h>
#include "CharacterStatistic.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Controllers/CharacterController.h"

namespace GameEngine
{
class GuiManager;
class GuiTextureElement;

namespace Components
{
class Player : public BaseComponent
{
    struct HudElements
    {
        struct Bar
        {
            int64& current;
            int64& maxValue;

            GuiTextureElement* texture{nullptr};
            int64 maxRendered{0};
            int64 currentRendered{0};

            void update();
        };

        Bar hp;
        Bar stamina;
        Bar mana;
    };

public:
    Player(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;

    void Init();
    void Update();
    void hurt(int64);

private:
    GuiManager& guiManager_;
    Animator* animator_;
    CharacterController* characterController_;
    CharacterStatistic characterStatistic_;
    HudElements hudElements_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
