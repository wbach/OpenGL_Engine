#include "WindowParams.h"

namespace GameEngine
{
namespace Params
{
namespace
{
const vec2ui DEFAULT_WINDOW_SIZE = {1000, 600};
}  // namespace

Window::Window()
    : size{IConfigurationParam::Type::Vec2i, DEFAULT_WINDOW_SIZE, {DEFAULT_WINDOW_SIZE}}
    , fullScreen{IConfigurationParam::Type::Boolean, false, {false, true}}
    , name{IConfigurationParam::Type::String, "Default Window Name", {}}
{
}
}  // namespace Params
}  // namespace GameEngine
