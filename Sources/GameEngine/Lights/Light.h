#pragma once
#include "LightTypes.h"
#include "Types.h"

class CLight
{	
public:
	CLight();
	CLight(vec3 colour);
	CLight(vec3 position, vec3 colour);
	CLight(vec3 position, vec3 colour, vec3 attenuation);
	CLight(vec3 position, vec3 colour, vec3 attenuation, float cutOff);
	void SetPosition(const vec3& position);
	void SetColor(const vec3& color);
	LightType::Type GetType() const;
	const vec3& GetPosition() const;
	const vec3& GetColour() const;
	const vec3& GetAttenuation() const;
	float GetCutoff() const;
	void SetType(LightType::Type type);

private:
    LightType::Type type;
    vec3 position;
    vec3 colour;
    vec3 attenuation;
    float cutOff;
};
