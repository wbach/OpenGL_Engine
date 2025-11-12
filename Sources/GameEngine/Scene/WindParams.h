#pragma once
#include <Types.h>

namespace GameEngine
{
struct WindParams
{
    vec2 windDirection{0.7f, 0.3f};  // kierunek wiatru w płaszczyźnie XZ (normalizowany wektor 2D)
    float windStrength{0.8};         // globalna siła wiatru; mnożnik amplitudy fal (im większe, tym mocniej wygina źdźbła)
    float bendAmount{0.7};           // maksymalny procent odchylenia dla źdźbła (kontrola "prostokątności" ruchu)
    float turbulence{1.0};           // wpływ różnych częstotliwości fal (szarpanie, nieregularność ruchu)
    float torsion{0.08};             // opcjonalna rotacja/skręt końcówek źdźbła ("torsja"), wprowadza lekki chaos
};

WindParams GetWindPreset(int preset);
}  // namespace GameEngine