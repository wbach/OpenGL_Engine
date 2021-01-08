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
    , waterReflectionResolution{IConfigurationParam::Type::Vec2i, {640, 480}, defaultResolutions()}
    , waterRefractionResolution{IConfigurationParam::Type::Vec2i, {320, 240}, defaultResolutions()}
{
}
}  // namespace Params
}  // namespace GameEngine
