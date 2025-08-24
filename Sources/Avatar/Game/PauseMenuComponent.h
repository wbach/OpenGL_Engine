#pragma once
#include <GameEngine/Components/BaseComponent.h>

#include "PauseMenu.h"

class PauseMenuComponent : public GameEngine::Components::BaseComponent
{
public:
    PauseMenuComponent(GameEngine::Components::ComponentContext&, GameEngine::GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;

private:
    std::unique_ptr<PauseMenu> pauseMenu;
    PauseMenu::State startState;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};

extern "C" void registerReadFunction();
