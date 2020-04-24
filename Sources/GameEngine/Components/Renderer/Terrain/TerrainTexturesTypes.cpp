#include "TerrainTexturesTypes.h"
#include "Logger/Log.h"

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
    return "Unknown type";
}

GameEngine::TerrainTextureType GameEngine::CreateFromString(const std::string& type)
{
    if (type == "heightmap") return GameEngine::TerrainTextureType::heightmap;
    if (type == "blendMap") return GameEngine::TerrainTextureType::blendMap;
    if (type == "normalmap") return GameEngine::TerrainTextureType::normalmap;
    if (type == "backgorundTexture") return GameEngine::TerrainTextureType::backgorundTexture;
    if (type == "backgorundTextureNormal") return GameEngine::TerrainTextureType::backgorundTextureNormal;
    if (type == "backgorundTextureDisplacement") return GameEngine::TerrainTextureType::backgorundTextureDisplacement;
    if (type == "redTexture") return GameEngine::TerrainTextureType::redTexture;
    if (type == "redTextureNormal") return GameEngine::TerrainTextureType::redTextureNormal;
    if (type == "redTextureDisplacement") return GameEngine::TerrainTextureType::redTextureDisplacement;
    if (type == "greenTexture") return GameEngine::TerrainTextureType::greenTexture;
    if (type == "greenTextureNormal") return GameEngine::TerrainTextureType::greenTextureNormal;
    if (type == "greenTextureDisplacement") return GameEngine::TerrainTextureType::greenTextureDisplacement;
    if (type == "blueTexture") return GameEngine::TerrainTextureType::blueTexture;
    if (type == "blueTextureNormal") return GameEngine::TerrainTextureType::blueTextureNormal;
    if (type == "blueTextureDisplacement") return GameEngine::TerrainTextureType::blueTextureDisplacement;
    ERROR_LOG("unknow terrainTextureType=" + type);
    return TerrainTextureType();
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
    return false;
}
