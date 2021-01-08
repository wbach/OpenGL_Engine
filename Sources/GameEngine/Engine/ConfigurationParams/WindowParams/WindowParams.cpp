#include "WindowParams.h"
#include "GameEngine/Engine/ConfigurationParams/PramsConstants.h"

namespace GameEngine
{
namespace Params
{
namespace
{
const vec2ui DEFAULT_WINDOW_SIZE = {1000, 600};
}  // namespace

Window::Window()
    : size{ IConfigurationParam::Type::Vec2i, DEFAULT_WINDOW_SIZE, defaultResolutions() }
    , fullScreen{IConfigurationParam::Type::Boolean, false, {false, true}}
    , name{IConfigurationParam::Type::String, "Default Window Name", {}}
{
}
}  // namespace Params
}  // namespace GameEngine
