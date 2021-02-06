#include "TerrainParam.h"

namespace GameEngine
{
namespace Params
{
Terrain::Terrain()
    : resolutionDivideFactor{1, {1, 2, 4, 8, 16}}
    , meshPartsCount{ 1, {1, 2, 4, 8, 16, 32, 64, 128} }
{
}
}  // namespace Params
}  // namespace GameEngine
