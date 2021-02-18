#include "FloraParams.h"

namespace GameEngine
{
namespace Params
{
Flora::Flora()
    : isEnabled{true, {false, true}}
    , viewDistance{200.f, {10.f, 25.f, 50.f, 100.f, 200.f, 400.f, 500.f, 750.f, 1000.f}}
{
}
}  // namespace Params
}  // namespace GameEngine
