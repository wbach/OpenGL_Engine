#include "TerrainTexturesTypes.h"

#include <Logger/Log.h>

#include <magic_enum/magic_enum.hpp>

void std::from_string(const std::string& type, GameEngine::TerrainTextureType& texture)
{
    if (auto value = magic_enum::enum_cast<GameEngine::TerrainTextureType>(type))
    {
        texture = *value;
    }
    else
    {
        LOG_ERROR << "unknow terrainTextureType=" << type;
    }
}

std::string std::to_string(GameEngine::TerrainTextureType type)
{
    return std::string(magic_enum::enum_name(type));
}

bool GameEngine::IsTerrainTextureType(const std::string& type)
{
    return magic_enum::enum_cast<GameEngine::TerrainTextureType>(type).has_value();
}

namespace GameEngine
{
std::optional<Color> convertPaintAbleTextureTypeToColor(TerrainTextureType type)
{
    switch (type)
    {
        case TerrainTextureType::redTexture:
            return Color(1.f, 0.f, 0.f, 0.f);
        case TerrainTextureType::greenTexture:
            return Color(0.f, 1.f, 0.f, 0.f);
        case TerrainTextureType::blueTexture:
            return Color(0.f, 0.f, 1.f, 0.f);
        case TerrainTextureType::alphaTexture:
            return Color(0.f, 0.f, 0.f, 1.f);
        case TerrainTextureType::backgroundTexture:
            return Color(0.f, 0.f, 0.f, 0.f);
        default:
            return std::nullopt;
    }
};
}  // namespace GameEngine
// clang-format on
