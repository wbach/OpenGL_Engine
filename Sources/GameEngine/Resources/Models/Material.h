#pragma once
#include <string>

#include "GameEngine/Resources/File.h"
#include "Types.h"

namespace GameEngine
{
class GeneralTexture;
class ITextureLoader;

struct Material
{
    std::string name{"noName"};

    Color ambient{vec4(0.f, 0.f, 0.f, 1.f)};
    Color diffuse{vec4(0.8f, 0.8f, 0.8f, 1.f)};
    Color specular{vec4(0.f, 0.f, 0.f, 1.f)};

    float shineDamper       = 0.f;
    float reflectivity      = 0.f;
    float indexOfRefraction = 0.f;

    bool isTransparency  = false;
    bool useFakeLighting = false;

    GeneralTexture* diffuseTexture      = nullptr;
    GeneralTexture* ambientTexture      = nullptr;
    GeneralTexture* specularTexture     = nullptr;
    GeneralTexture* normalTexture       = nullptr;
    GeneralTexture* displacementTexture = nullptr;
};

using MaterialsMap = std::unordered_map<std::string, File>;
Material ParseMaterial(const File&, ITextureLoader&);
void SaveMaterial(const Material&, const File&);
}  // namespace GameEngine
