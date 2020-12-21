#pragma once
#include <Types.h>
#include "WaterType.h"
#include "../../ConfigurationParam.h"

namespace GameEngine
{
namespace Params
{
struct Water
{
    Water();
    ConfigurationParam<WaterType> type;
    ConfigurationParam<vec2ui> waterReflectionResolution;
    ConfigurationParam<vec2ui> waterRefractionResolution;
};
}  // namespace Params
}  // namespace GameEngine
