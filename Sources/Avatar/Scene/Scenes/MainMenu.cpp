#include "MainMenu.h"

#include <GLM/GLMUtils.h>
#include <Thread.hpp>

#include <GameEngine/Renderers/GUI/GuiRenderer.h>
#include <GameEngine/Renderers/GUI/Text/GuiTextRenderer.h>
#include <GameEngine/Resources/Textures/HeightMap.h>
#include <GameEngine/Resources/Textures/Image.h>

#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>

using namespace GameEngine;

namespace AvatarGame
{
MainMenu::MainMenu()
    : Scene("MainMenu")
{
}

MainMenu::~MainMenu()
{
    DEBUG_LOG("");
}

int MainMenu::Initialize()
{
    DEBUG_LOG("");
    return 0;
}

void MainMenu::PostInitialize()
{
}

int MainMenu::Update(float)
{
    return 0;
}

}  // namespace AvatarGame
