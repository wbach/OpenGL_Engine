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
    ConfigurationParam<bool> limitTextureSize;
    ConfigurationParam<bool> useDisplacement;
    ConfigurationParam<bool> useAmbient;
    ConfigurationParam<bool> useDiffuse;
    ConfigurationParam<bool> useNormal;
    ConfigurationParam<bool> useSpecular;
    ConfigurationParam<bool> useRoughness;
    ConfigurationParam<bool> useMetallic;
    ConfigurationParam<bool> useAmientOcclusion;
    ConfigurationParam<bool> useOpacity;
};
}  // namespace Params
}  // namespace GameEngine
