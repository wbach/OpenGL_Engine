#include "Light.h"

CLight::CLight(glm::vec3 position, glm::vec3 colour, glm::vec3 attenuation, float cutOff)
: m_Type(LightType::SPOT_LIGHT)
, m_Position(position)
, m_Colour(colour)
, m_Attenuation(attenuation)
, m_CutOff(cutOff)
{
}


CLight::CLight(glm::vec3 position,glm::vec3 colour,glm::vec3 attenuation)
: m_Type(LightType::POINT_LIGHT)
, m_Position(position)
, m_Colour(colour)
, m_Attenuation(attenuation)
, m_CutOff(0)
{
}

CLight::CLight(glm::vec3 position,glm::vec3 colour)
: m_Type(LightType::DIRECTIONAL_LIGHT)
, m_Position(position)
, m_Colour(colour)
, m_Attenuation(glm::vec3(1, 0, 0))
, m_CutOff(0)
{
}
CLight::CLight(glm::vec3 colour)
: m_Type(LightType::DIRECTIONAL_LIGHT)
, m_Position(100000, 150000, 100000)
, m_Colour(colour)
, m_Attenuation(glm::vec3(1, 0, 0))
, m_CutOff(0)
{
}
CLight::CLight()
: m_Type(LightType::DIRECTIONAL_LIGHT)
, m_Position(100000, 150000, 100000)
, m_Colour(0.8)
, m_Attenuation(glm::vec3(1, 0, 0))
, m_CutOff(0)
{
}

const int& CLight::GetType() const
{
	return m_Type;
}

const glm::vec3& CLight::GetPosition() const
{
	return m_Position;
}

const glm::vec3& CLight::GetColour() const
{
	return m_Colour;
}

const glm::vec3& CLight::GetAttenuation() const
{
	return m_Attenuation;
}

const float& CLight::GetCutoff() const
{
	return m_CutOff;
}
void CLight::SetPosition(const glm::vec3& position)
{
	m_Position = position;
}

void CLight::SetColor(const glm::vec3 & color)
{
	m_Colour = color;
}
