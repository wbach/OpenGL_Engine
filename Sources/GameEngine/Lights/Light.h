#pragma once
#include "LightTypes.h"
#include "Types.h"

namespace GameEngine
{
class Light
{
public:
    Light();
    Light(vec3 colour);
    Light(vec3 position, vec3 colour);
    Light(vec3 position, vec3 colour, vec3 attenuation);
    Light(vec3 position, vec3 colour, vec3 attenuation, float cutOff);
    void SetPosition(const vec3& position);
    void SetColor(const vec3& color);
    LightType GetType() const;
    const vec3& GetPosition() const;
    const vec3& GetColour() const;
    const vec3& GetAttenuation() const;
    vec3 GetDirection() const;
    float GetCutoff() const;
    void SetType(LightType type);

private:
    vec3 direction;
    vec3 position;
    vec3 colour;
    vec3 attenuation;
    float cutOff;
    LightType type;
};
}  // namespace GameEngine
