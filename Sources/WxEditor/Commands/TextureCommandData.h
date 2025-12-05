#pragma once
#include <Utils/Image/Image.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"

struct TextureCommandData
{
    GameEngine::Components::TerrainRendererComponent& component;
    Utils::ImageData snapshot;
    Utils::ImageData current;
};