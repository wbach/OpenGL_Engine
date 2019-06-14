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
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&](){ addEngineEvent(EngineEvent::QUIT);});

    DEBUG_LOG("");

    guiManager_->RegisterAction("StartGame()", [&]() {
        SceneEvent sceneEvent(SceneEventType::LOAD_SCENE_BY_ID, 1);
        addSceneEvent(sceneEvent);
    });

    guiManager_->RegisterAction("LoadGame()", []() { DEBUG_LOG("Load game button pressed"); });

    guiManager_->RegisterAction("Settings()", [&]() {
        DEBUG_LOG("Settings button pressed");
        auto startWindow = guiManager_->Get<GuiWindowElement>("StartWindow");
        if (startWindow)
        {
            startWindow->Hide();
        }

        auto settingsWindow = guiManager_->Get<GuiWindowElement>("SettingsWindow");
        if (settingsWindow)
        {
            settingsWindow->Show();
        }
    });

    guiManager_->RegisterAction("Exit()", [&]() { /*DEBUG_LOG("Exit game button pressed"); */
                                                  addEngineEvent(EngineEvent::QUIT);
    });

    guiManager_->RegisterAction("BackToMainMenu()", [&]() {
        auto settingsWindow = guiManager_->Get<GuiWindowElement>("SettingsWindow");
        if (settingsWindow)
        {
            settingsWindow->Hide();
        }

        auto startWindow = guiManager_->Get<GuiWindowElement>("StartWindow");
        if (startWindow)
        {
            startWindow->Show();
        }
    });

    guiElementFactory_->ReadGuiFile("MainMenuGui.xml");

    return 0;
}

void MainMenu::PostInitialize()
{
}

int MainMenu::Update(float)
{
    guiElementFactory_->ReadGuiFile("MainMenuGui.xml");
    return 0;
}

}  // namespace AvatarGame
