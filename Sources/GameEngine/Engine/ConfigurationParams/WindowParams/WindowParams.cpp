#include "WindowParams.h"
#include "GameEngine/Engine/ConfigurationParams/PramsConstants.h"

namespace GameEngine
{
namespace Params
{
Window::Window()
    : size{DEFAULT_WINDOW_SIZE, defaultResolutions()}
    , fullScreen{false, {false, true}}
    , name{"Default Window Name", {}}
{
}
}  // namespace Params
}  // namespace GameEngine
