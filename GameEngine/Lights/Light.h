#pragma once
#include "LightTypes.h"
#include <glm/glm.hpp>

class CLight
{	
public:
	CLight();
	CLight(glm::vec3 colour);
	CLight(glm::vec3 position, glm::vec3 colour);
	CLight(glm::vec3 position, glm::vec3 colour, glm::vec3 attenuation);
	CLight(glm::vec3 position, glm::vec3 colour, glm::vec3 attenuation, float cutOff);
	void SetPosition(const glm::vec3& position);
	void SetColor(const glm::vec3& color);
	LightType::Type GetType() const;
	const glm::vec3& GetPosition() const;
	const glm::vec3& GetColour() const;
	const glm::vec3& GetAttenuation() const;
	float GetCutoff() const;
	void SetType(LightType::Type type);

private:
    LightType::Type type;
    glm::vec3 position;
    glm::vec3 colour;
    glm::vec3 attenuation;
    float cutOff;
};
