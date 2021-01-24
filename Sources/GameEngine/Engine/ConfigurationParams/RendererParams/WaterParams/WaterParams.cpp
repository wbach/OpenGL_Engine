#include "WaterParams.h"
#include "GameEngine/Engine/ConfigurationParams/PramsConstants.h"

namespace GameEngine
{
namespace Params
{
Water::Water()
    : type{WaterType::FULL, {WaterType::SIMPLE, WaterType::REFLECTED_REFRACTED, WaterType::FULL}}
    , waterReflectionResolution{DEFAULT_WINDOW_SIZE, defaultResolutions()}
    , waterRefractionResolution{DEFAULT_WINDOW_SIZE, defaultResolutions()}
{
}
}  // namespace Params
}  // namespace GameEngine
