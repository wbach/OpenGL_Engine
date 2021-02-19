#include "GreenIsland.h"

#include <GLM/GLMUtils.h>
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Engine/ConfigurationExplorer.h>
#include <GameEngine/Engine/ConfigurationWriter.h>
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/GuiRenderer.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <GameEngine/Renderers/RenderersManager.h>
#include <GameEngine/Resources/ResourceManager.h>
#include <GameEngine/Resources/Textures/HeightMap.h>
#include <GameEngine/Renderers/GUI/Layer/DefaultLayers.h>
#include <Logger/Log.h>
#include "Avatar/Game/PauseMenu.h"
#include "PauseMenuTheme.h"
#include <Thread.hpp>

using namespace GameEngine;

namespace AvatarGame
{
GreenIsland::GreenIsland()
    : Scene("GreenIsland")
{
}

GreenIsland::~GreenIsland()
{
    DEBUG_LOG(__FUNCTION__);
    menu_.reset();
}

int GreenIsland::Initialize()
{
    DEBUG_LOG("GreenIsland::Initialize()");

    prepareMenu();
    keyOperations();

    const std::string sceneFile = EngineConf_GetFullDataPath("Scenes/GreenIsland/GreenIsland.xml");
    LoadFromFile(sceneFile);

    DEBUG_LOG("GreenIsland::Initialized");
    return 0;
}

void GreenIsland::PostInitialize()
{
}

int GreenIsland::Update(float)
{
    return 0;
}
void GreenIsland::prepareMenu()
{
    guiElementFactory_->SetTheme(getGuiTheme());

    guiManager_->RegisterAction("BackToMainMenu()", [&](auto&) {
        SceneEvent sceneEvent(SceneEventType::LOAD_SCENE_BY_ID, 0);
        addSceneEvent(sceneEvent);
    });
    guiManager_->RegisterAction("ExitGame()", [&](auto&) { addEngineEvent(EngineEvent::QUIT); });

    menu_ = std::make_unique<PauseMenu>(*this, *guiElementFactory_, *guiManager_);
}

void GreenIsland::keyOperations()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::F1, [&]() { addEngineEvent(EngineEvent::QUIT); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::P, [this]() { renderersManager_->GetDebugRenderer().Enable(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::O, [this]() { renderersManager_->GetDebugRenderer().Disable(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { DEBUG_LOG("Escape pressed"); menu_->isShow() ? menu_->hide() : menu_->show(PauseMenu::State::PauseMenu); });

}
}  // namespace AvatarGame
