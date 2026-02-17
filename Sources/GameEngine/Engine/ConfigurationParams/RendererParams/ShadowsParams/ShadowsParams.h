#pragma once
#include <Types.h>

#include "GameEngine/Engine/ConfigurationParams/ConfigurationEnumParam.h"
#include "GameEngine/Engine/ConfigurationParams/ConfigurationParam.h"

namespace GameEngine
{
namespace Params
{
const uint32 MAX_SHADOW_MAP_CASADES{4};

struct Shadows
{
    enum class CascadeDistanceFunc
    {
        linear,
        quadratic,
        exp
    };

    Shadows();
    ConfigurationParam<bool> isEnabled;
    ConfigurationParam<bool> useFullTrees;
    ConfigurationParam<float> distance;
    ConfigurationParam<uint32> mapSize;
    ConfigurationParam<float> firstCascadeDistance;
    ConfigurationParam<uint32> cascadesSize;
    ConfigurationEnumParam<CascadeDistanceFunc> cascadeDistanceFunc;
};
}  // namespace Params
}  // namespace GameEngine
