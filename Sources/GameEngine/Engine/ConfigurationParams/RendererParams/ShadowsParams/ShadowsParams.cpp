#include "ShadowsParams.h"

namespace GameEngine
{
namespace Params
{
Shadows::Shadows()
    : isEnabled{true, {false, true}}
    , distance{400.f, {100.f, 200.f, 400.f}}
    , mapSize{4096, {128, 256, 512, 1024, 2048, 4096, 8192}}
    , firstCascadeDistance{25.f, {50.f, 75.f}}
    , cascadesSize{MAX_SHADOW_MAP_CASADES, {1, 2, 3, MAX_SHADOW_MAP_CASADES}}
    , cascadeDistanceFunc{CascadeDistanceFunc::quadratic}
{
}
}  // namespace Params
}  // namespace GameEngine
