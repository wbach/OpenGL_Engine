#pragma once
#include <Types.h>
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include <Utils/Image/Image.h>

namespace GameEngine
{
struct PaintTextureBasedContext
{
    Components::TerrainRendererComponent* terrainComponent;
    std::reference_wrapper<Utils::Image> blendmapImageData;
    Color paintedColor;
    vec3 terrainPosition;
    vec3 terrainScale;
};
}  // namespace GameEngine