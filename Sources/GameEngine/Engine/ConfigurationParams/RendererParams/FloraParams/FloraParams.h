#pragma once
#include <Types.h>

#include "../../ConfigurationParam.h"

namespace GameEngine
{
namespace Params
{
struct Flora
{
    Flora();
    ConfigurationParam<bool> isEnabled;
    ConfigurationParam<float> viewDistance;
};
}  // namespace Params
}  // namespace GameEngine
