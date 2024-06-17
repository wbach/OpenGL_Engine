#include "SouthPole.h"

#include <GLM/GLMUtils.h>
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Engine/ConfigurationExplorer.h>
#include <GameEngine/Engine/ConfigurationWriter.h>
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/GuiRenderer.h>
#include <GameEngine/Renderers/GUI/Layer/DefaultLayers.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <GameEngine/Renderers/RenderersManager.h>
#include <GameEngine/Resources/ResourceManager.h>
#include <GameEngine/Resources/Textures/HeightMap.h>
#include <Logger/Log.h>

#include <Thread.hpp>

#include "Avatar/Game/PauseMenu.h"
#include "PauseMenuTheme.h"

using namespace GameEngine;

namespace AvatarGame
{
SouthPole::SouthPole()
    : Scene("SouthPole")
{
}

SouthPole::~SouthPole()
{
    DEBUG_LOG(__FUNCTION__);
    menu_.reset();
}

int SouthPole::Initialize()
{
    DEBUG_LOG("SouthPole::Initialize()");

    prepareMenu();
    keyOperations();

    const std::string sceneFile = EngineConf_GetFullDataPath("Scenes/SouthPole/SouthPole.xml");
    LoadFromFile(sceneFile);

    DEBUG_LOG("SouthPole::Initialized");
    return 0;
}

void SouthPole::PostInitialize()
{
}

int SouthPole::Update(float)
{
    return 0;
}
void SouthPole::prepareMenu()
{
    guiElementFactory_->SetTheme(getGuiTheme());

    guiManager_->RegisterAction("BackToMainMenu()", [&](auto&) {
        SceneEvent sceneEvent(SceneEventType::LOAD_SCENE_BY_ID, 0);
        addSceneEvent(sceneEvent);
    });
    guiManager_->RegisterAction("ExitGame()", [&](auto&) { addEngineEvent(EngineEvent::QUIT); });

    menu_ = std::make_unique<PauseMenu>(PauseMenu::State::PauseMenu, *this, *guiElementFactory_, *guiManager_);
}

void SouthPole::keyOperations()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::F1, [&]() { addEngineEvent(EngineEvent::QUIT); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::P, [this]() { renderersManager_->GetDebugRenderer().Enable(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::O, [this]() { renderersManager_->GetDebugRenderer().Disable(); });
}
}  // namespace AvatarGame
