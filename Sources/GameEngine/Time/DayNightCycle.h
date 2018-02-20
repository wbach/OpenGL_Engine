#pragma once
#include "../Lights/Light.h"
#include "GameTime.h"
#include "Types.h"

class CDayNightCycle : public CGameTime
{
public:
	CDayNightCycle();
	void Update(const float& delta_time) override;
	void UpdateSunColor();
	void UpdateSunPosition();
	void CalculateBlendFactor();
	void SetDirectionalLight(CLight* light);

	const wb::vec2i GetCurrentHour() const;
	const bool IsDay() const;
	const bool IsNight() const;
	const bool IsMorning() const;
	const bool IsEvening() const;
	const bool IsFirstHalfMorning() const;
	const bool IsFirstHalfEvening() const;
	float GetDayNightBlendFactor() const;

private:
    CLight*   directionalLight;
    vec3 sunRiseColor;
    vec3 midDayColor;
    vec3 sunSetColor;
    vec3 nightColor;

    float dayNightBlendFactor;
    float dayStart;
    float dayEnd;
    float nightStart;
    float nightEnd;
    float morningDuration;
    float eveningDuration;

    vec3 defaultSunPos;
};

inline float CDayNightCycle::GetDayNightBlendFactor() const
{
    return dayNightBlendFactor;
}
