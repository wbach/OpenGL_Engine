#pragma once
#include <unordered_map>

namespace GameEngine
{
class Texture;
enum class TerrainTextureType
{
    blendMap = 1,
    normalmap,
    heightmap,
    backgorundTexture,
    backgorundTextureNormal,
    backgorundTextureDisplacement,
    redTexture,
    redTextureNormal,
    redTextureDisplacement,
    greenTexture,
    greenTextureNormal,
    greenTextureDisplacement,
    blueTexture,
    blueTextureNormal,
    blueTextureDisplacement
};
typedef std::unordered_map<TerrainTextureType, Texture*> TerrainTexturesMap;
}  // namespace GameEngine
