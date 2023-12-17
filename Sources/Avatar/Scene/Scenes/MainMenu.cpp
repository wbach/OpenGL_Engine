#include "MainMenu.h"

#include <GLM/GLMUtils.h>
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/GuiRenderer.h>
#include <GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <GameEngine/Resources/Textures/HeightMap.h>

#include <Thread.hpp>

#include "Avatar/Game/PauseMenu.h"
#include "PauseMenuTheme.h"

using namespace GameEngine;

namespace AvatarGame
{
MainMenu::MainMenu(const std::unordered_map<std::string, uint32>& avaiableScenes)
    : Scene("MainMenu")
    , avaiableScenes_{avaiableScenes}
{
}
MainMenu::~MainMenu()
{
    DEBUG_LOG("");
}

int MainMenu::Initialize()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });
    prepareMenu();
    return 0;
}

void MainMenu::PostInitialize()
{
}

int MainMenu::Update(float)
{
    return 0;
}

void MainMenu::prepareMenu()
{
    guiElementFactory_->SetTheme(getGuiTheme());

    auto backgorund = guiElementFactory_->CreateGuiTexture("GUI/204473.jpg");
    backgorund->SetZPosition(100.f);
    guiManager_->Add(std::move(backgorund));

    guiManager_->RegisterAction("StartGame()", [&](auto&) {
        SceneEvent sceneEvent(SceneEventType::LOAD_SCENE_BY_ID, 3);
        addSceneEvent(sceneEvent);
    });
    guiManager_->RegisterAction("LoadGame()", [&](auto&) {});
    guiManager_->RegisterAction("ExitGame()", [&](auto&) { addEngineEvent(EngineEvent::QUIT); });

    std::vector<std::string> scenes;
    scenes.reserve(avaiableScenes_.size());
    for(const auto&[name, _] : avaiableScenes_)
        scenes.push_back(name);

    menu_ = std::make_unique<PauseMenu>(*this, *guiElementFactory_, *guiManager_, scenes);
    menu_->show(PauseMenu::State::MainMenu);
}
}  // namespace AvatarGame
