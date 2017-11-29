#pragma once
#include <string>
#include <vector>
#include "../Textures/Texture.h"
#include "Types.h"

struct SMaterial
{
    std::string name;

    vec3 ambient  = vec3(0);
    vec3 diffuse  = vec3(0.8);
    vec3 specular = vec3(0);

    float shineDamper       = 0.f;
    float reflectivity      = 0.f;
    float indexOfRefraction = 0.f;

    bool isTransparency  = false;
    bool useFakeLighting = false;

    CTexture* diffuseTexture  = nullptr;
    CTexture* ambientTexture  = nullptr;
    CTexture* specularTexture = nullptr;
    CTexture* normalTexture   = nullptr;

    int numberOfRows = 1;
    int textureIndex = 0;

    float getTextureXOffset()
    {
        int column = textureIndex % numberOfRows;
        return (float)column / (float)numberOfRows;
    }
    float getTextureYOffset()
    {
        int row = textureIndex / numberOfRows;
        return (float)row / (float)numberOfRows;
    }
};
