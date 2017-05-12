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
	CLight*   m_DirectionalLight;
	glm::vec3 m_SunRiseColor;
	glm::vec3 m_MidDayColor;
	glm::vec3 m_SunSetColor;
	glm::vec3 m_NightColor;

	float m_DayNightBlendFactor;
	float m_DayStart;
	float m_DayEnd;
	float m_NightStart;
	float m_NightEnd;
	float m_MorningDuration;
	float m_EveningDuration;

	glm::vec3 m_DefaultSunPos;
};
inline float CDayNightCycle::GetDayNightBlendFactor() const
{
	return m_DayNightBlendFactor;
}