#pragma once
#include <string>
#include <Types.h>

namespace GameEngine
{
enum class TerrainTextureType
{
    heightmap                     = 1,
    blendMap                      = 2,
    normalmap                     = 3,
    backgroundTexture             = 4,
    backgroundTextureNormal       = 5,
    backgroundTextureDisplacement = 6,
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

};
class Texture;
bool IsTerrainTextureType(const std::string&);
std::optional<Color> convertPaintAbleTextureTypeToColor(TerrainTextureType);
}  // namespace GameEngine
namespace std
{
void from_string(const std::string& type, GameEngine::TerrainTextureType&);
std::string to_string(GameEngine::TerrainTextureType);
}  // namespace std
