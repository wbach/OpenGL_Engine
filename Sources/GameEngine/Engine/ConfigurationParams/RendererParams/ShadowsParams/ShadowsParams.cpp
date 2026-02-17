#include "ShadowsParams.h"

namespace GameEngine
{
namespace Params
{
Shadows::Shadows()
    : isEnabled{true, {false, true}}
    , useFullTrees{false, {false, true}}
    , distance{400.f, {5.f, 10.f, 25.f, 50.f, 100.f, 200.f, 400.f, 500.f}}
    , mapSize{4096, {128, 256, 512, 1024, 2048, 4096, 8192}}
    , firstCascadeDistance{25.f, {1.f, 2.f, 3.f, 5.f, 8.f, 13.f, 21.f, 35.f, 50.f, 75.f}}
    , cascadesSize{MAX_SHADOW_MAP_CASADES, {1, 2, 3, MAX_SHADOW_MAP_CASADES}}
    , cascadeDistanceFunc{CascadeDistanceFunc::quadratic}
{
}
}  // namespace Params
}  // namespace GameEngine
