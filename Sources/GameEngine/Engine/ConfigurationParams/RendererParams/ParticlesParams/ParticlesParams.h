#pragma once
#include <Types.h>

#include "../../ConfigurationParam.h"

namespace GameEngine
{
namespace Params
{
struct ENGINE_API Particles
{
    Particles();
    ConfigurationParam<bool> useParticles;
};
}  // namespace Params
}  // namespace GameEngine
