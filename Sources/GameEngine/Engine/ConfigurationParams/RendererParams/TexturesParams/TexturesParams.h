#pragma once
#include <Types.h>

#include "../../ConfigurationParam.h"

namespace GameEngine
{
namespace Params
{
struct Textures
{
    Textures();
    ConfigurationParam<vec2ui> maxSize;
    ConfigurationParam<bool> useDisplacement;
    ConfigurationParam<bool> useAmbient;
    ConfigurationParam<bool> useDiffuse;
    ConfigurationParam<bool> useNormal;
    ConfigurationParam<bool> useSpecular;
};
}  // namespace Params
}  // namespace GameEngine
