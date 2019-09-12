#include "MainMenu.h"

#include <GLM/GLMUtils.h>
#include <Thread.hpp>

#include <GameEngine/Renderers/GUI/GuiRenderer.h>
#include <GameEngine/Resources/Textures/HeightMap.h>
#include <GameEngine/Resources/Textures/Image.h>

#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>

#include <GameEngine/Engine/Configuration.h>

using namespace GameEngine;

namespace AvatarGame
{
std::string mainMenuGuiFile;

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
    mainMenuGuiFile = EngineConf_GetFullDataPath("Scenes/MainMenu/MainMenuGui.xml");

    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });

    DEBUG_LOG("");

    guiManager_->RegisterAction("StartGame()", [&](auto&) {
        SceneEvent sceneEvent(SceneEventType::LOAD_SCENE_BY_ID, 1);
        addSceneEvent(sceneEvent);
    });

    guiManager_->RegisterAction("LoadGame()", [&](auto&) { DEBUG_LOG("Load game button pressed"); stateToChange_ = State::LOAD_GAME; });

    guiManager_->RegisterAction("Settings()", [&](auto&) {
        DEBUG_LOG("Settings button pressed");
        stateToChange_ = State::SETTINGS;
    });

    guiManager_->RegisterAction("Exit()", [&](auto&) { /*DEBUG_LOG("Exit game button pressed"); */
                                                  addEngineEvent(EngineEvent::QUIT);
    });

    guiManager_->RegisterAction("BackToMainMenu()", [&](auto&) {
        stateToChange_ = State::MAIN;
    });

    guiElementFactory_->ReadGuiFile(mainMenuGuiFile);

    return 0;
}

void MainMenu::PostInitialize()
{
}

int MainMenu::Update(float)
{
    if (stateToChange_)
    {
        ChangeState(*stateToChange_);
        stateToChange_.reset();
    }
    return 0;
}

void MainMenu::ChangeState(State state)
{
    auto startWindow    = guiManager_->Get<GuiWindowElement>("StartWindow");
    auto settingsWindow = guiManager_->Get<GuiWindowElement>("SettingsWindow");
    auto loadingWindow = guiManager_->Get<GuiWindowElement>("LoadGameWindow");

    if (state == State::SETTINGS)
    {
        if (loadingWindow)
        {
            loadingWindow->Hide();
        }
        if (startWindow)
        {
            startWindow->Hide();
        }
        if (settingsWindow)
        {
            settingsWindow->Show();
        }
    }
    else if (state == State::MAIN)
    {
        if (settingsWindow)
        {
            settingsWindow->Hide();
        }
        if (startWindow)
        {
            startWindow->Show();
        }
        if (loadingWindow)
        {
            loadingWindow->Hide();
        }
    }
    else if (state == State::LOAD_GAME)
    {
        if (settingsWindow)
        {
            settingsWindow->Hide();
        }
        if (startWindow)
        {
            startWindow->Hide();
        }
        if (loadingWindow)
        {
            loadingWindow->Show();
        }
    }
}

}  // namespace AvatarGame
