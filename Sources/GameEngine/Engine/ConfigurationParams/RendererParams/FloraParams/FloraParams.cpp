#include "FloraParams.h"

namespace GameEngine
{
namespace Params
{
Flora::Flora()
    : isEnabled{IConfigurationParam::Type::Boolean, false, {false, true}}
    , viewDistance{IConfigurationParam::Type::Float, 100.f, {50.f, 100.f, 200.f, 400.f}}
{
}
}  // namespace Params
}  // namespace GameEngine
