#pragma once
#include "Types.h"

namespace GameEngine
{
namespace TerrainDef
{
static const float PART_SIZE       = 16.0f;
static const float SIZE            = 64.f;
static const float VSCALE          = 10.f;
static const float HEIGHT_FACTOR   = 4.f * PART_SIZE;
static const float TOTAL_SIZE      = SIZE;
static const float HALF_TOTAL_SIZE = (TOTAL_SIZE * VSCALE) / 2.f;

static const uint32 TERRAIN_ROOT_NODES_COUNT = 8;
}  // namespace TerrainDef
}  // namespace GameEngine
