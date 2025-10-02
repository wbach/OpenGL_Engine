#pragma once
#include "GameEngine/Resources/File.h"
#include "TerrainTexturesTypes.h"

namespace GameEngine
{
class HeightMap;

namespace Components
{
struct TerrainTexture
{
    File file;
    float tiledScale{1.f};
    TerrainTextureType type;
};

}  // namespace Components
}  // namespace GameEngine