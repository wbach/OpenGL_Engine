#pragma once
#include <Types.h>

#include "GameEngine/Engine/ConfigurationParams/ConfigurationParam.h"

namespace GameEngine
{
namespace Params
{
struct Ssao
{
    Ssao();
    ConfigurationParam<bool> isEnabled;
    ConfigurationParam<float> resolutionDevider;
    ConfigurationParam<float> radius;
    ConfigurationParam<float> bias;
};
}  // namespace Params
}  // namespace GameEngine
