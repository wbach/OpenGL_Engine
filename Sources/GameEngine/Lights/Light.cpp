#include "Light.h"
#include "GameEngine/Lights/LightTypes.h"

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
    CalculateDirection(glm::vec3(0.0f, 0.0f, 0.0f));
}
Light::Light(vec3 colour)
    : type(LightType::DIRECTIONAL_LIGHT)
    , position(100000.f, 150000.f, 100000.f)
    , colour(colour)
    , attenuation(vec3(1.f, 0.f, 0.f))
    , cutOff(0.f)
{
    CalculateDirection(glm::vec3(0.0f, 0.0f, 0.0f));
}
Light::Light()
    : type(LightType::DIRECTIONAL_LIGHT)
    , position(100000.f, 150000.f, 100000.f)
    , colour(.8f)
    , attenuation(vec3(1.f, 0.f, 0.f))
    , cutOff(0.f)
{
    CalculateDirection(glm::vec3(0.0f, 0.0f, 0.0f));
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
    return direction;
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
    if (type == LightType::DIRECTIONAL_LIGHT)
    {
        CalculateDirection(glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void Light::SetColor(const vec3& c)
{
    colour = c;
}
void Light::CalculateDirection(const glm::vec3& targetPosition)
{
    //direction = glm::normalize(targetPosition - position);
    direction = -1.f * glm::normalize(position);
}
}  // namespace GameEngine
