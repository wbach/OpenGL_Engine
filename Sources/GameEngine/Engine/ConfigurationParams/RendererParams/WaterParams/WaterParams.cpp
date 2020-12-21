#include "WaterParams.h"

namespace GameEngine
{
namespace Params
{
namespace
{
std::vector<vec2ui> resolutions()
{
    return {{320, 240}, {640, 480}, {1280, 800}, {1366, 768}, {1920, 1080}, {2560, 1080}};
}
}  // namespace

Water::Water()
    : type{IConfigurationParam::Type::Enum,
           WaterType::FULL,
           {WaterType::SIMPLE, WaterType::REFLECTED_REFRACTED, WaterType::FULL}}
    , waterReflectionResolution{IConfigurationParam::Type::Vec2i, {640, 480}, resolutions()}
    , waterRefractionResolution{IConfigurationParam::Type::Vec2i, {320, 240}, resolutions()}
{
}
}  // namespace Params
}  // namespace GameEngine
