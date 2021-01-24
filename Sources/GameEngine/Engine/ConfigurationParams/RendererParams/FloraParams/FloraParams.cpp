#include "FloraParams.h"

namespace GameEngine
{
namespace Params
{
Flora::Flora()
    : isEnabled{false, {false, true}}
    , viewDistance{100.f, {50.f, 100.f, 200.f, 400.f}}
{
}
}  // namespace Params
}  // namespace GameEngine
