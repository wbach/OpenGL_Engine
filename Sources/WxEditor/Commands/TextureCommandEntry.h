#pragma once
#include <Utils/Image/Image.h>

#include <vector>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"

struct TextureCommandEntry
{
    GameEngine::Components::TerrainRendererComponent& component;
    Utils::ImageData snapshot;
};

using TextureCommandEntries = std::vector<TextureCommandEntry>;