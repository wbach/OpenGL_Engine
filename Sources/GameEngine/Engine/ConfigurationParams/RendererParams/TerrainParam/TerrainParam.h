#pragma once
#include "TerrainType.h"
#include "GameEngine/Engine/ConfigurationParams/ConfigurationParam.h"

namespace GameEngine
{
namespace Params
{
struct Terrain
{
    Terrain();
    TerrainType terrainType = TerrainType::Mesh;
    ConfigurationParam<uint32> resolutionDivideFactor;
    ConfigurationParam<uint32> meshPartsCount;
};
}  // namespace Params
}  // namespace GameEngine
