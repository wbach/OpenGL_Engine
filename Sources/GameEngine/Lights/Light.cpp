#include "Light.h"

namespace GameEngine
{
Light::Light(vec3 position, vec3 colour, vec3 attenuation, float cutOff)
    : type(LightType::SPOT_LIGHT)
    , position(position)
    , colour(colour)
    , attenuation(attenuation)
    , cutOff(cutOff)
{
}

Light::Light(vec3 position, vec3 colour, vec3 attenuation)
    : type(LightType::POINT_LIGHT)
    , position(position)
    , colour(colour)
    , attenuation(attenuation)
    , cutOff(0.f)
{
}

Light::Light(vec3 position, vec3 colour)
    : type(LightType::DIRECTIONAL_LIGHT)
    , position(position)
    , colour(colour)
    , attenuation(vec3(1.f, 0.f, 0.f))
    , cutOff(0.f)
{
    direction = -1.f * glm::normalize(position);
}
Light::Light(vec3 colour)
    : type(LightType::DIRECTIONAL_LIGHT)
    , position(100000.f, 150000.f, 100000.f)
    , colour(colour)
    , attenuation(vec3(1.f, 0.f, 0.f))
    , cutOff(0.f)
{
    direction = -1.f * glm::normalize(position);
}
Light::Light()
    : type(LightType::DIRECTIONAL_LIGHT)
    , position(100000.f, 150000.f, 100000.f)
    , colour(.8f)
    , attenuation(vec3(1.f, 0.f, 0.f))
    , cutOff(0.f)
{
    direction = -1.f * glm::normalize(position);
}

LightType Light::GetType() const
{
    return type;
}

const vec3& Light::GetPosition() const
{
    return position;
}

const vec3& Light::GetColour() const
{
    return colour;
}

const vec3& Light::GetAttenuation() const
{
    return attenuation;
}

vec3 Light::GetDirection() const
{
    return type == LightType::DIRECTIONAL_LIGHT ? position * -1.f : direction;
}

float Light::GetCutoff() const
{
    return cutOff;
}

void Light::SetType(LightType t)
{
    type = t;
}
void Light::SetPosition(const vec3& pos)
{
    position = pos;
}

void Light::SetColor(const vec3& c)
{
    colour = c;
}
}  // namespace GameEngine
