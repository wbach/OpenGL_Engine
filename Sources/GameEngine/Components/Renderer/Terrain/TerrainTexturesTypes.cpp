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
// clang-format on
