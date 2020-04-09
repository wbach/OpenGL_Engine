#pragma once
#include <string>
#include <vector>
#include "../Textures/Texture.h"
#include "Types.h"

namespace GameEngine
{
struct Material
{
    std::string name;

    vec3 ambient  = vec3(0.f);
    vec3 diffuse  = vec3(.8f);
    vec3 specular = vec3(0.f);

    float shineDamper       = 0.f;
    float reflectivity      = 0.f;
    float indexOfRefraction = 0.f;

    bool isTransparency  = false;
    bool useFakeLighting = false;

    Texture* diffuseTexture  = nullptr;
    Texture* ambientTexture  = nullptr;
    Texture* specularTexture = nullptr;
    Texture* normalTexture   = nullptr;
};
}  // namespace GameEngine
