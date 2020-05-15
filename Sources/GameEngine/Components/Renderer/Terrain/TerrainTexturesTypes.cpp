#include "TerrainTexturesTypes.h"

#include "Logger/Log.h"

// clang-format off
#define FROM_STRING(x)  if (type == #x) {texture = GameEngine::TerrainTextureType::x; return;}

void std::from_string(const std::string& type, GameEngine::TerrainTextureType& texture)
{
    FROM_STRING(heightmap);
    FROM_STRING(blendMap);
    FROM_STRING(normalmap);
    FROM_STRING(backgorundTexture);
    FROM_STRING(backgorundTextureNormal);
    FROM_STRING(backgorundTextureDisplacement);
    FROM_STRING(redTexture);
    FROM_STRING(redTextureNormal);
    FROM_STRING(redTextureDisplacement);
    FROM_STRING(greenTexture);
    FROM_STRING(greenTextureNormal);
    FROM_STRING(greenTextureDisplacement);
    FROM_STRING(blueTexture);
    FROM_STRING(blueTextureNormal);
    FROM_STRING(blueTextureDisplacement);
    FROM_STRING(alphaTexture);
    FROM_STRING(alphaTextureNormal);
    FROM_STRING(alphaTextureDisplacement);
    ERROR_LOG("unknow terrainTextureType=" + type);
}
#undef FROM_STRING

std::string std::to_string(GameEngine::TerrainTextureType type)
{
    if (type == GameEngine::TerrainTextureType::heightmap) return "heightmap";
    if (type == GameEngine::TerrainTextureType::blendMap) return "blendMap";
    if (type == GameEngine::TerrainTextureType::normalmap) return "normalmap";
    if (type == GameEngine::TerrainTextureType::backgorundTexture) return "backgorundTexture";
    if (type == GameEngine::TerrainTextureType::backgorundTextureNormal) return "backgorundTextureNormal";
    if (type == GameEngine::TerrainTextureType::backgorundTextureDisplacement) return "backgorundTextureDisplacement";
    if (type == GameEngine::TerrainTextureType::redTexture) return "redTexture";
    if (type == GameEngine::TerrainTextureType::redTextureNormal) return "redTextureNormal";
    if (type == GameEngine::TerrainTextureType::redTextureDisplacement) return "redTextureDisplacement";
    if (type == GameEngine::TerrainTextureType::greenTexture) return "greenTexture";
    if (type == GameEngine::TerrainTextureType::greenTextureNormal) return "greenTextureNormal";
    if (type == GameEngine::TerrainTextureType::greenTextureDisplacement) return "greenTextureDisplacement";
    if (type == GameEngine::TerrainTextureType::blueTexture) return "blueTexture";
    if (type == GameEngine::TerrainTextureType::blueTextureNormal) return "blueTextureNormal";
    if (type == GameEngine::TerrainTextureType::blueTextureDisplacement) return "blueTextureDisplacement";
    if (type == GameEngine::TerrainTextureType::alphaTexture) return "alphaTexture";
    if (type == GameEngine::TerrainTextureType::alphaTextureNormal) return "alphaTextureNormal";
    if (type == GameEngine::TerrainTextureType::alphaTextureDisplacement) return "alphaTextureDisplacement";
    return "Unknown type";
}

bool GameEngine::IsTerrainTextureType(const std::string& type)
{
    if (type == "heightmap") return true;
    if (type == "blendMap") return true;
    if (type == "normalmap") return true;
    if (type == "backgorundTexture") return true;
    if (type == "backgorundTextureNormal") return true;
    if (type == "backgorundTextureDisplacement") return true;
    if (type == "redTexture") return true;
    if (type == "redTextureNormal") return true;
    if (type == "redTextureDisplacement") return true;
    if (type == "greenTexture") return true;
    if (type == "greenTextureNormal") return true;
    if (type == "greenTextureDisplacement") return true;
    if (type == "blueTexture") return true;
    if (type == "blueTextureNormal") return true;
    if (type == "blueTextureDisplacement") return true;
    if (type == "alphaTexture") return true;
    if (type == "alphaTextureNormal") return true;
    if (type == "alphaTextureDisplacement") return true;
    return false;
}
// clang-format on
