#include "WaterParams.h"

#include "GameEngine/Engine/ConfigurationParams/PramsConstants.h"

namespace GameEngine
{
namespace Params
{
Water::Water()
    : type{WaterType::FULL, {WaterType::SIMPLE, WaterType::REFLECTED_REFRACTED, WaterType::FULL}}
    , renderDistance{200.f, {50.f, 100.f, 200.f, 400.f, 800.f, 1000.f, 1200.f, 1500.f, 2000.f, 3000.f, 4000.f, 5000.f, 10000.f, -1.f}}
    , waterReflectionResolution{DEFAULT_WINDOW_SIZE, defaultResolutions()}
    , waterRefractionResolution{DEFAULT_WINDOW_SIZE, defaultResolutions()}
{
}
}  // namespace Params
}  // namespace GameEngine
