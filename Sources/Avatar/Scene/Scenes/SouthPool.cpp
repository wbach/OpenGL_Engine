#include "SouthPool.h"

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

    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&](){ addEngineEvent(EngineEvent::QUIT);});
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
