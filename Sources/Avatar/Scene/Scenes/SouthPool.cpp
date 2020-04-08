#include "SouthPool.h"

#include <GLM/GLMUtils.h>
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/GuiRenderer.h>
#include <GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <GameEngine/Renderers/RenderersManager.h>
#include <GameEngine/Resources/Textures/HeightMap.h>
#include <GameEngine/Resources/Textures/Image.h>
#include <GameEngine/Resources/ResourceManager.h>

#include <Thread.hpp>

using namespace GameEngine;

namespace AvatarGame
{
const std::string pauseMenuFile = "Scenes/PauseMenu/PauseMenu.xml";

SouthPool::SouthPool()
    : Scene("SouthPool")
{
}

SouthPool::~SouthPool()
{
    DEBUG_LOG(__FUNCTION__);
}

int SouthPool::Initialize()
{
    DEBUG_LOG("SouthPool::Initialize()");

    //resourceManager_->GetTextureLaoder().CreateHeightMap("Textures/Terrain/HeightMaps/World.png",
    //                                                     "Textures/Terrain/HeightMaps/output.terrain", vec3(1.f));

    inputManager_->SubscribeOnKeyDown(KeyCodes::F1, [&]() { addEngineEvent(EngineEvent::QUIT); });

    const std::string sceneFile = EngineConf_GetFullDataPath("Scenes/SouthPool/SouthPool.xml");
    LoadFromFile(sceneFile);

    inputManager_->SubscribeOnKeyDown(KeyCodes::P, [this]() { renderersManager_->DisableDebugRenderer(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::O, [this]() { renderersManager_->EnableDebugRenderer(); });

    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() {
        auto window = guiManager_->GetElement("PauseMenuMainWindow");
        if (window)
        {
            if (window->IsShow())
            {
                window->Hide();
                camera.Unlock();
            }
            else
            {
                window->Show();
                camera.Lock();
            }
        }
        else
        {
            camera.Unlock();
        }
    });

    guiManager_->RegisterAction("BackToMainMenu()", [&](auto&) {
        SceneEvent sceneEvent(SceneEventType::LOAD_SCENE_BY_ID, 0);
        addSceneEvent(sceneEvent);
    });

    guiManager_->RegisterAction("Resume()", [&](auto&) {
        auto window = guiManager_->GetElement("PauseMenuMainWindow");
        if (window)
        {
            if (window->IsShow())
            {
                window->Hide();
                camera.Unlock();
            }
            else
            {
                window->Show();
                camera.Lock();
            }
        }
    });

    guiManager_->RegisterAction("ExitGame()", [&](auto&) { addEngineEvent(EngineEvent::QUIT); });

    guiElementFactory_->ReadGuiFile(EngineConf_GetFullDataPath(pauseMenuFile));

    DEBUG_LOG("SouthPool::Initialized");
    return 0;
}

void SouthPool::PostInitialize()
{
}

int SouthPool::Update(float)
{
    return 0;
}
}  // namespace AvatarGame
