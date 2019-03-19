#pragma once
#include <unordered_map>

namespace GameEngine
{
class Texture;
enum class TerrainTextureType
{
    blendMap = 1,
    backgorundTexture,
    backgorundTextureNormal,
    redTexture,
    redTextureNormal,
    greenTexture,
    greenTextureNormal,
    blueTexture,
    blueTextureNormal,
    rockTexture,
    rockNormalTexture,
    snowTexture,
    snowNormalTexture,
    heightmap,
};
typedef std::unordered_map<TerrainTextureType, Texture*> TerrainTexturesMap;
}  // namespace GameEngine
