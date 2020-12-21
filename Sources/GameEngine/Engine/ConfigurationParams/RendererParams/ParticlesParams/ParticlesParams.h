#pragma once
#include <Types.h>

#include "../../ConfigurationParam.h"

namespace GameEngine
{
namespace Params
{
struct Particles
{
    Particles();
    ConfigurationParam<bool> useParticles;
};
}  // namespace Params
}  // namespace GameEngine
