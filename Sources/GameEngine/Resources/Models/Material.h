#pragma once
#include <string>

#include "GameEngine/Resources/File.h"
#include "Types.h"

/*
==============================================================================
Material Structure – opis parametrów (Legacy → PBR, AAA-ready)
==============================================================================

name
    Nazwa materiału (np. "LeafSet024", "StoneWall", "Glass_Clear")

------------------------------------------------------------------------------
LEGACY – Phong / Blinn (tymczasowe, do migracji)
------------------------------------------------------------------------------

ambient
    Kolor ambient (stary model oświetlenia, NIE PBR)

diffuse
    Kolor diffuse (legacy albedo)

specular
    Kolor refleksu specular (legacy model)

shineDamper
    Połysk / ostrość refleksu (legacy shininess)

reflectivity
    Intensywność odbić (legacy, fallback dla metallicFactor)

indexOfRefraction
    Współczynnik załamania (szkło, woda)

isTransparency
    Materiał przezroczysty (blend, legacy)

useFakeLighting
    Ignoruje normalne / światło (legacy trick)

diffuseTexture
    Tekstura diffuse / albedo (legacy)

ambientTexture
    Pseudo-ambient (legacy, fallback dla AO)

specularTexture
    Tekstura specular (legacy)

------------------------------------------------------------------------------
PBR – Physically Based Rendering (docelowe)
------------------------------------------------------------------------------

baseColor
    Kolor bazowy (albedo) + alpha

metallicFactor
    Metaliczność:
        0.0 = dielektryk (liście, drewno, plastik)
        1.0 = metal (stal, złoto)

roughnessFactor
    Szorstkość powierzchni:
        0.0 = gładkie lustro
        1.0 = całkowicie matowe

ambientOcclusion
    Wartość AO (ciemniejsze zagłębienia)

normalScale
    Skala normal mapy (1.0 = pełny efekt)

opacityCutoff
    Próg alpha test (discard), zakres 0.0–1.0
    Używane np. dla liści, siatek, ogrodzeń

subsurfaceStrength
    Siła subsurface scattering (fake SSS, foliage / skóra)

subsurfaceColor
    Kolor SSS:
        zielony – liście
        czerwony – skóra

------------------------------------------------------------------------------
PBR Textures
------------------------------------------------------------------------------

baseColorTexture
    Tekstura baseColor / albedo

normalTexture
    Normal map (konwencja zgodna z API renderera)

metallicTexture
    Tekstura metaliczności

roughnessTexture
    Tekstura szorstkości

ambientOcclusionTexture
    Tekstura ambient occlusion

opacityTexture
    Tekstura alpha (cutout / przezroczystość)

displacementTexture
    Height / displacement / parallax (opcjonalnie)

------------------------------------------------------------------------------
Material Flags (bitmask)
------------------------------------------------------------------------------

materialFlags
    Specjalne zachowania materiału:

        MAT_DOUBLE_SIDED      – render dwustronny
        MAT_ALPHA_TEST       – alpha test (discard)
        MAT_ALPHA_BLEND      – alpha blending (szkło, woda)
        MAT_FOLIAGE          – liście / wiatr / SSS
        MAT_UNLIT            – materiał nieoświetlany
        MAT_RECEIVE_SHADOW   – materiał przyjmuje cienie

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

    // Phong legacy
    Color ambient{vec4(0.f, 0.f, 0.f, 1.f)};
    Color diffuse{vec4(1.f, 1.f, 1.f, 1.f)};
    Color specular{vec4(0.f, 0.f, 0.f, 1.f)};

    float shineDamper       = 0.f;
    float reflectivity      = 0.f;
    float indexOfRefraction = 0.f;

    bool isTransparency  = false;
    bool useFakeLighting = false;

    GeneralTexture* diffuseTexture  = nullptr;
    GeneralTexture* ambientTexture  = nullptr;
    GeneralTexture* specularTexture = nullptr;

    // PBR – Physically Based Rendering
    glm::vec4 baseColor{1.f, 1.f, 1.f, 1.f};
    float metallicFactor   = 0.0f;
    float roughnessFactor  = 0.7f;
    float ambientOcclusion = 1.0f;

    float tiledScale         = 1.0f;
    float normalScale        = 1.0f;
    float opacityCutoff      = 0.5f;
    float subsurfaceStrength = 0.0f;
    glm::vec3 subsurfaceColor{0.0f};

    GeneralTexture* baseColorTexture        = nullptr;
    GeneralTexture* normalTexture           = nullptr;
    GeneralTexture* metallicTexture         = nullptr;
    GeneralTexture* roughnessTexture        = nullptr;
    GeneralTexture* ambientOcclusionTexture = nullptr;
    GeneralTexture* opacityTexture          = nullptr;
    GeneralTexture* displacementTexture     = nullptr;

    uint32_t flags = 0;
};

using MaterialsMap = std::unordered_map<std::string, File>;
Material ParseMaterial(const File&, ITextureLoader&);
void SaveMaterial(const Material&, const File&);
}  // namespace GameEngine
