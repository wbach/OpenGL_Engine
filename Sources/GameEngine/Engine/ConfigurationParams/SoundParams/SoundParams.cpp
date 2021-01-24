#include "SoundParams.h"

namespace GameEngine
{
namespace Params
{
Sound::Sound()
    : isEnabled{false, {false, true}}
    , volume{1.0f, {0.f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.f}}
{
}
}  // namespace Params
}  // namespace GameEngine
