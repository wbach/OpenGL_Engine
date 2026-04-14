#pragma once
#include <optional>

#include "Bar.h"
#include "CharacterStatistic.h"
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace GUI
{
class Manager;
class Window;
}  // namespace GUI

namespace Components
{
class Animator;
class CharacterController;

class Enemy : public BaseComponent
{
    struct HudElements
    {
        Bar hp;
        GUI::Window* window{nullptr};
    };

public:
    Enemy(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void Init();
    void Update();
    std::optional<int64> hurt(int64);
    const CharacterStatistic& characterStatistic() const;

private:
    void isOnGround();

private:
    GUI::Manager& guiManager_;
    Animator* animator_;
    CharacterController* characterController_;
    CharacterStatistic characterStatistic_;
    HudElements hud_;
    std::vector<IdType> animSubs_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
