#pragma once
#include <Types.h>

#include "../ConfigurationParam.h"

namespace GameEngine
{
namespace Params
{
struct ENGINE_API Sound
{
    Sound();
    ConfigurationParam<bool> isEnabled;
    ConfigurationParam<float> volume;
    bool isEnabledTmpNotSerialization{true}; // Nie zapisujemy w konfiguracji, tylko tymczasowe uzycie.
};
}  // namespace Params
}  // namespace GameEngine
