#include "ShadowsParams.h"

namespace GameEngine
{
namespace Params
{
Shadows::Shadows()
    : isEnabled{IConfigurationParam::Type::Boolean, true, {false, true}}
    , distance{IConfigurationParam::Type::Float, 400.f, {100.f, 200.f, 400.f}}
    , mapSize{IConfigurationParam::Type::Integer, 4096, {128, 256, 512, 1024, 2048, 4096, 8192}}
    , firstCascadeDistance{IConfigurationParam::Type::Float, 25.f, {50.f, 75.f}}
    , cascadesSize{IConfigurationParam::Type::Integer, MAX_SHADOW_MAP_CASADES, {1, 2, 3, MAX_SHADOW_MAP_CASADES}}
    , cascadeDistanceFunc{CascadeDistanceFunc::quadratic}
{
}
}  // namespace Params
}  // namespace GameEngine
