#pragma once
#include <Types.h>

#include "GameEngine/Engine/ConfigurationParams/ConfigurationParam.h"

namespace GameEngine
{
namespace Params
{
struct Lightshafts
{
    Lightshafts();
    ConfigurationParam<bool> isEnabled;
    ConfigurationParam<float> resolutionDevider;
};
}  // namespace Params
}  // namespace GameEngine
