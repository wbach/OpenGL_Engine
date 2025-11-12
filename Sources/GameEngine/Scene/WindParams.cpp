#include "WindParams.h"

namespace GameEngine
{
WindParams GetWindPreset(int preset)
{
    WindParams wp;

    if (preset == 0)  // lekki wiatr
    {
        wp.windDirection = normalize(vec2(0.5, 0.2));
        wp.windStrength  = 0.2;
        wp.bendAmount    = 0.3;
        wp.turbulence    = 0.8;
        wp.torsion       = 0.02;
    }
    else if (preset == 1)  // średni wiatr
    {
        wp.windDirection = normalize(vec2(0.7, 0.3));
        wp.windStrength  = 0.5;
        wp.bendAmount    = 0.5;
        wp.turbulence    = 1.0;
        wp.torsion       = 0.05;
    }
    else if (preset == 2)  // wichura
    {
        wp.windDirection = normalize(vec2(1.0, 0.4));
        wp.windStrength  = 0.8;
        wp.bendAmount    = 0.7;
        wp.turbulence    = 1.5;
        wp.torsion       = 0.08;
    }
    else
    {
        // domyślny preset = lekki
        wp.windDirection = normalize(vec2(0.5, 0.2));
        wp.windStrength  = 0.2;
        wp.bendAmount    = 0.3;
        wp.turbulence    = 0.8;
        wp.torsion       = 0.02;
    }

    return wp;
}
}  // namespace GameEngine