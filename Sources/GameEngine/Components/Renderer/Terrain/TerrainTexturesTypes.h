#pragma once
#include <string>
#include <unordered_map>

namespace GameEngine
{
enum class TerrainTextureType
{
    // shadowMap0                 = 0,
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
    blueTextureDisplacement       = 15,
    alphaTexture                  = 16,
    alphaTextureNormal            = 17,
    alphaTextureDisplacement      = 18,
    rockTexture                   = 19,
    rockTextureNormal             = 20,
    rockTextureDisplacement       = 21
    // shadowMap1                 = 22,
    // shadowMap2                 = 23,
    // shadowMap3                 = 24,

};
class Texture;
bool IsTerrainTextureType(const std::string&);
}  // namespace GameEngine
namespace std
{
void from_string(const std::string& type, GameEngine::TerrainTextureType&);
std::string to_string(GameEngine::TerrainTextureType);
}  // namespace std
