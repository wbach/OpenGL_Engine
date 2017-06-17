#include "Light.h"

CLight::CLight(vec3 position, vec3 colour, vec3 attenuation, float cutOff)
: type(LightType::SPOT_LIGHT)
, position(position)
, colour(colour)
, attenuation(attenuation)
, cutOff(cutOff)
{
}


CLight::CLight(vec3 position,vec3 colour,vec3 attenuation)
: type(LightType::POINT_LIGHT)
, position(position)
, colour(colour)
, attenuation(attenuation)
, cutOff(0)
{
}

CLight::CLight(vec3 position,vec3 colour)
: type(LightType::DIRECTIONAL_LIGHT)
, position(position)
, colour(colour)
, attenuation(vec3(1, 0, 0))
, cutOff(0)
{
}
CLight::CLight(vec3 colour)
: type(LightType::DIRECTIONAL_LIGHT)
, position(100000, 150000, 100000)
, colour(colour)
, attenuation(vec3(1, 0, 0))
, cutOff(0)
{
}
CLight::CLight()
: type(LightType::DIRECTIONAL_LIGHT)
, position(100000, 150000, 100000)
, colour(0.8)
, attenuation(vec3(1, 0, 0))
, cutOff(0)
{
}

LightType::Type CLight::GetType() const
{
    return type;
}

const vec3& CLight::GetPosition() const
{
    return position;
}

const vec3& CLight::GetColour() const
{
    return colour;
}

const vec3& CLight::GetAttenuation() const
{
    return attenuation;
}

float CLight::GetCutoff() const
{
    return cutOff;
}

void CLight::SetType(LightType::Type t)
{ 
    type = t;
}
void CLight::SetPosition(const vec3& pos)
{
    position = pos;
}

void CLight::SetColor(const vec3 & c)
{
    colour = c;
}
