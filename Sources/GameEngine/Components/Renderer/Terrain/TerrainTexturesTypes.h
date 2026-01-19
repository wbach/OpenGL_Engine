#pragma once
#include <string>
#include <Types.h>

namespace GameEngine
{
enum class TerrainTextureType
{
    blendMap                      = 0,
    backgroundTexture             = 1,
    backgroundTextureNormal       = 2,
    backgroundTextureDisplacement = 3,
    redTexture                    = 4,
    redTextureNormal              = 5,
    redTextureDisplacement        = 6,
    greenTexture                  = 7,
    greenTextureNormal            = 8,
    greenTextureDisplacement      = 9,
    blueTexture                   = 10,
    blueTextureNormal             = 11,
    blueTextureDisplacement       = 12,
    alphaTexture                  = 13,
    alphaTextureNormal            = 14,
    alphaTextureDisplacement      = 15,
    rockTexture                   = 16,
    rockTextureNormal             = 17,
    rockTextureDisplacement       = 18,
    heightmap                     = 19,
    normalmap                     = 20
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
