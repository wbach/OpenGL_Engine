#include "WaterParams.h"
#include "GameEngine/Engine/ConfigurationParams/PramsConstants.h"

namespace GameEngine
{
namespace Params
{
Water::Water()
    : type{IConfigurationParam::Type::Enum,
           WaterType::FULL,
           {WaterType::SIMPLE, WaterType::REFLECTED_REFRACTED, WaterType::FULL}}
    , waterReflectionResolution{IConfigurationParam::Type::Vec2i, DEFAULT_WINDOW_SIZE, defaultResolutions()}
    , waterRefractionResolution{IConfigurationParam::Type::Vec2i, DEFAULT_WINDOW_SIZE, defaultResolutions()}
{
}
}  // namespace Params
}  // namespace GameEngine
