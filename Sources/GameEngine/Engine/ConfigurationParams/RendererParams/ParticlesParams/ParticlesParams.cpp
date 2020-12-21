#include "ParticlesParams.h"

namespace GameEngine
{
namespace Params
{
Particles::Particles()
    : useParticles{IConfigurationParam::Type::Boolean, false, {false, true}}
{
}
}  // namespace Params
}  // namespace GameEngine
