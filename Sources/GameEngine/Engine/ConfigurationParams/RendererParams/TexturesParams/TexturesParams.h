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
    bool useDisplacement;
    bool useAmbient;
    bool useDiffuse;
    bool useNormal;
    bool useSpecular;
};
}  // namespace Params
}  // namespace GameEngine
