#pragma once
#include <string>
#include "glm/glm.hpp"
#include <vector>
#include "../Textures/Texture.h"

struct SMaterial
{
	std::string name;

	glm::vec3 ambient  = glm::vec3(0);
	glm::vec3 diffuse  = glm::vec3(0.8);
	glm::vec3 specular = glm::vec3(0);

	float shineDamper  = 0.f;
	float reflectivity = 0.f;
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
		int column = textureIndex%numberOfRows;
		return (float)column / (float)numberOfRows;
	}
	float getTextureYOffset()
	{
		int row = textureIndex / numberOfRows;
		return (float)row / (float)numberOfRows;
	}
};
