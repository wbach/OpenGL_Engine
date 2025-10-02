#pragma once
#include "Bar.h"
#include "CharacterStatistic.h"
#include "GameEngine/Components/BaseComponent.h"

namespace common
{
class Transform;
}

namespace GameEngine
{
class GuiManager;
class GuiTextureElement;
class GuiWindowElement;

namespace Components
{
class Animator;
class CharacterController;

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
    void Reload() override;

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
