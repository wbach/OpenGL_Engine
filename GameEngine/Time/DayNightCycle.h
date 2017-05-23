#pragma once
#include "../Lights/Light.h"
#include "GameTime.h"

class CDayNightCycle : public CGameTime
{
public:
	CDayNightCycle();
	void Update(const float& delta_time) override;
	void UpdateSunColor();
	void UpdateSunPosition();
	void CalculateBlendFactor();
	void SetDirectionalLight(CLight* light);

	const void GetCurrentHour(int& hour, int& minutes) const;
	const bool IsDay() const;
	const bool IsNight() const;
	const bool IsMorning() const;
	const bool IsEvening() const;
	const bool IsFirstHalfMorning() const;
	const bool IsFirstHalfEvening() const;
	float GetDayNightBlendFactor() const;

private:
    CLight*   directionalLight;
    glm::vec3 sunRiseColor;
    glm::vec3 midDayColor;
    glm::vec3 sunSetColor;
    glm::vec3 nightColor;

    float dayNightBlendFactor;
    float dayStart;
    float dayEnd;
    float nightStart;
    float nightEnd;
    float morningDuration;
    float eveningDuration;

    glm::vec3 defaultSunPos;
};

inline float CDayNightCycle::GetDayNightBlendFactor() const
{
    return dayNightBlendFactor;
}
