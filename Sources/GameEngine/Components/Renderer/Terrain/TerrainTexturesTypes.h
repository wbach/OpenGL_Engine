
#pragma once
#include <unordered_map>

namespace GameEngine
{
class Texture;
enum class TerrainTextureType
{
    //shadowMap                   = 0
    heightmap                     = 1,
    blendMap                      = 2,
    normalmap                     = 3,
    backgorundTexture             = 4,
    backgorundTextureNormal       = 5,
    backgorundTextureDisplacement = 6,
    redTexture                    = 7,
    redTextureNormal              = 8,
    redTextureDisplacement        = 9,
    greenTexture                  = 10,
    greenTextureNormal            = 11,
    greenTextureDisplacement      = 12,
    blueTexture                   = 13,
    blueTextureNormal             = 14,
    blueTextureDisplacement       = 15
};
typedef std::unordered_map<TerrainTextureType, Texture*> TerrainTexturesMap;
}  // namespace GameEngine
