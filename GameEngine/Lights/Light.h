#pragma once
#include "glm/glm.hpp"
namespace LightType
{
	enum {
		DIRECTIONAL_LIGHT = 0,
		POINT_LIGHT,
		SPOT_LIGHT
	};
}

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
	const int& GetType() const;
	const glm::vec3& GetPosition() const;
	const glm::vec3& GetColour() const;
	const glm::vec3& GetAttenuation() const;
	const float& GetCutoff() const;
	void SetType(int type) { m_Type = type; }
private:
	int m_Type;
	glm::vec3 m_Position;
	glm::vec3 m_Colour;
	glm::vec3 m_Attenuation;
	float m_CutOff;
};