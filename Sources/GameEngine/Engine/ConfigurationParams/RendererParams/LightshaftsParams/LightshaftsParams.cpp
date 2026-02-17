#include "LightshaftsParams.h"

namespace GameEngine
{
namespace Params
{
Lightshafts::Lightshafts()
    : isEnabled{true, {false, true}}
    , resolutionDevider{1.f, {1.f, 1.33f, 2.0f, 4.0f}}
{
}
}  // namespace Params
}  // namespace GameEngine
