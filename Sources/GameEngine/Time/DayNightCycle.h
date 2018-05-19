#pragma once
#include "Types.h"
#include "GameTime.h"
#include "../Lights/Light.h"

namespace GameEngine
{
	class DayNightCycle : public GameTime
	{
	public:
		DayNightCycle();
		void Update(const float& delta_time) override;
		void UpdateSunColor();
		void UpdateSunPosition();
		void CalculateBlendFactor();
		void SetDirectionalLight(Light* light);

		const wb::vec2i GetCurrentHour() const;
		const bool IsDay() const;
		const bool IsNight() const;
		const bool IsMorning() const;
		const bool IsEvening() const;
		const bool IsFirstHalfMorning() const;
		const bool IsFirstHalfEvening() const;
		float GetDayNightBlendFactor() const;

	private:
		Light* directionalLight;
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

	inline float DayNightCycle::GetDayNightBlendFactor() const
	{
		return dayNightBlendFactor;
	}

} //GameEngine
