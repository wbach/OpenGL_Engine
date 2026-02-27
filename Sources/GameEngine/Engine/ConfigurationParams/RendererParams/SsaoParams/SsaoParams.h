#pragma once
#include <Types.h>

#include "GameEngine/Engine/ConfigurationParams/ConfigurationParam.h"

namespace GameEngine
{
namespace Params
{
struct ENGINE_API Ssao
{
    Ssao();
    ConfigurationParam<bool> isEnabled;
    ConfigurationParam<float> resolutionDevider;
    ConfigurationParam<float> radius;
    ConfigurationParam<float> bias;
};
}  // namespace Params
}  // namespace GameEngine
