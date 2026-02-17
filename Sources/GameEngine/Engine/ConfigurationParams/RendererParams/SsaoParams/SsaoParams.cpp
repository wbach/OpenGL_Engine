#include "SsaoParams.h"

namespace GameEngine
{
namespace Params
{
Ssao::Ssao()
    : isEnabled{true, {false, true}}
    , resolutionDevider{1.f, {1.f, 1.33f, 2.0f, 4.0f}}
    , radius{0.5f, {0.1, 0.25, 0.5, 1.0, 1.5, 2.0, 3.5, 5.0}}
    , bias{0.025, {0.001, 0.005, 0.01, 0.025, 0.05, 0.1, 0.2}}
{
}
}  // namespace Params
}  // namespace GameEngine
