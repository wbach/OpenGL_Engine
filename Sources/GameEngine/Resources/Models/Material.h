#pragma once
#include <string>

#include "GameEngine/Resources/File.h"
#include "Types.h"

/*
==============================================================================
Material Structure – Physically Based Rendering (PBR, AAA-ready)
==============================================================================

name
    Nazwa materiału (np. "LeafSet024", "StoneWall", "Glass_Clear")

------------------------------------------------------------------------------
PBR – Surface Parameters
------------------------------------------------------------------------------

baseColor
    Kolor bazowy (Albedo) w przestrzeni liniowej + Alpha w kanale W.

metallicFactor
    Stopień metaliczności powierzchni (0.0 = dielektryk, 1.0 = metal).

roughnessFactor
    Mikro-szorstkość (0.0 = lustro, 1.0 = pełny mat).
    Wpływa na rozproszenie światła specular.

indexOfRefraction (IOR)
    Współczynnik załamania światła (np. powietrze 1.0, woda 1.33, szkło 1.5).
    Kluczowe dla materiałów z flagą MAT_ALPHA_BLEND.

occlusionStrength
    Statyczne zacienienie wnęk (0.0 = całkowicie czarne, 1.0 = brak cienia).

normalScale
    Siła wypukłości mapy normalnych (zazwyczaj 1.0).

opacityCutoff
    Próg odcięcia dla flagi MAT_ALPHA_TEST (discard piksela).

subsurfaceStrength / subsurfaceColor
    Siła i kolor rozproszenia podpowierzchniowego (SSS).
    Używane głównie dla roślinności (MAT_FOLIAGE) i skóry.

------------------------------------------------------------------------------
PBR Textures
------------------------------------------------------------------------------

baseColorTexture
    Główna mapa koloru (RGB) i opcjonalnie alpha (A).

normalTexture
    Mapa normalnych (Tangent Space).

metallicRoughnessTexture
    Mapa skojarzona (często pakowana: R=AO, G=Roughness, B=Metallic).

occlusionTexture
    Osobna mapa Ambient Occlusion (jeśli nie jest częścią pakietu wyżej).

opacityTexture
    Osobna mapa maski alpha (używana, gdy alpha nie jest w baseColor).

emissionTexture
    Mapa emisyjna (RGB) – światło własne materiału.

displacementTexture
    Mapa wysokości (Height/Parallax Mapping).

------------------------------------------------------------------------------
Material Flags (bitmask)
------------------------------------------------------------------------------

flags (uint32_t)
    Specjalne stany renderowania:

    MAT_DOUBLE_SIDED   – Wyłącza Backface Culling (liście, tkaniny).
    MAT_ALPHA_TEST     – Używa opacityCutoff do maskowania (liście, płoty).
    MAT_ALPHA_BLEND    – Przezroczystość typu Alpha-to-Coverage lub Translucency.
    MAT_FOLIAGE        – Specjalny model cieniowania (wiatr, silniejszy SSS).
    MAT_UNLIT          – Pomija obliczenia oświetlenia (Emissive/UI/FakeLighting).
    MAT_RECEIVE_SHADOW – Czy na powierzchni mają być rysowane cienie.

==============================================================================
*/

enum MaterialFlags : uint32_t
{
    MAT_DOUBLE_SIDED   = 1 << 0,  // foliage
    MAT_ALPHA_TEST     = 1 << 1,  // liście
    MAT_ALPHA_BLEND    = 1 << 2,  // szkło
    MAT_FOLIAGE        = 1 << 3,  // SSS + special lighting
    MAT_UNLIT          = 1 << 4,
    MAT_RECEIVE_SHADOW = 1 << 5
};

namespace GameEngine
{
class GeneralTexture;
class ITextureLoader;

struct Material
{
    std::string name{"noName"};

    Color baseColor{1.f, 1.f, 1.f, 1.f};
    float metallicFactor    = 0.0f;
    float roughnessFactor   = 0.7f;
    float normalScale       = 1.0f;
    float occlusionStrength = 1.0f;

    float indexOfRefraction = 1.0f;
    float transmission      = 0.0f;
    float opacityCutoff     = 0.5f;

    vec3 subsurfaceColor{0.0f};
    float subsurfaceStrength = 0.0f;

    float uvScale = 1.0f;

    GeneralTexture* baseColorTexture    = nullptr;
    GeneralTexture* normalTexture       = nullptr;
    GeneralTexture* metallicTexture     = nullptr;
    GeneralTexture* roughnessTexture    = nullptr;
    GeneralTexture* occlusionTexture    = nullptr;
    GeneralTexture* emissionTexture     = nullptr;
    GeneralTexture* opacityTexture      = nullptr;
    GeneralTexture* displacementTexture = nullptr;

    uint32_t flags = 0;
};

using MaterialsMap = std::unordered_map<std::string, File>;
Material ParseMaterial(const File&, ITextureLoader&);
void SaveMaterial(const Material&, const File&);
}  // namespace GameEngine
