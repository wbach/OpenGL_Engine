#pragma once
#include <GameEngine/Components/BaseComponent.h>

#include "PauseMenu.h"

using namespace GameEngine;
using namespace GameEngine::Components;

namespace GameEngine
{
class GuiTextureElement;
}

class PauseMenuComponent : public BaseComponent
{
public:
    File pauseMenuImage;
    File mainMenuBackground;
    PauseMenu::State startState;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_TEXTURE(pauseMenuImage)
        FIELD_TEXTURE(mainMenuBackground)
        FIELD_ENUM(startState)
    END_FIELDS()
public:
    PauseMenuComponent(GameEngine::Components::ComponentContext&, GameEngine::GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;

private:
    std::unique_ptr<PauseMenu> pauseMenu;
    GuiTextureElement* backgroundTexture;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};

extern "C" void registerReadFunction();
