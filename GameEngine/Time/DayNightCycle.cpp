#include "DayNightCycle.h"
#include "../Utils/GLM/GLMUtils.h"
#include "../Debug_/Log.h"
#include <GL/glew.h>

CDayNightCycle::CDayNightCycle()
    : CGameTime()
    , directionalLight(nullptr)
    , sunRiseColor(Utils::RGBtoFloat(253.f, 168.f, 87.f))
    , midDayColor(1.f, 0.784314f, 0.588235)
    , sunSetColor(Utils::RGBtoFloat(253.f, 94.f, 83.f))
    , nightColor(glm::vec3(.01f))
    , dayNightBlendFactor(0.5f)
    , dayStart(.45f)
    , dayEnd(.75f)
    , nightStart(.95f)
    , nightEnd(0.25f)
{
    morningDuration = dayStart - nightEnd;
    eveningDuration = nightStart - dayEnd;
    defaultSunPos = glm::vec3(10000, 15000, 10000);
}
void CDayNightCycle::Update(const float & delta_time)
{
	CGameTime::Update(delta_time);
	CalculateBlendFactor();
	UpdateSunColor();
    UpdateSunPosition();
}

void CDayNightCycle::UpdateSunColor()
{
    if (directionalLight == nullptr)
	{
		Log("Directional light not set in CDayNightCycle but is used.");
		return;
	}
	if (IsNight())
	{
        directionalLight->SetColor(nightColor);
	}
	if (IsDay())
	{
        directionalLight->SetColor(midDayColor);
	}
	if (IsMorning())
	{
		glm::vec3 color;
        float half_moorning_duration = morningDuration / 2.f;
		if (IsFirstHalfMorning())
		{
            float r = (currentTime - nightEnd) * 1.f / half_moorning_duration;
            color = Utils::ColorLerpRGB(nightColor, sunRiseColor, r);
		}
		else
		{
            float r = (currentTime - nightEnd - half_moorning_duration) * 1.f / half_moorning_duration;
            color = Utils::ColorLerpRGB(sunRiseColor, midDayColor, r);
		}
        directionalLight->SetColor(color);
	}
	if (IsEvening())
	{
		glm::vec3 color;
        float half_evening = eveningDuration / 2.f;
		if (IsFirstHalfEvening())
		{
            float r = (currentTime - dayEnd) * 1.f / half_evening;
            color = Utils::ColorLerpRGB(midDayColor, sunSetColor, r);
		}
		else
		{
            float r = (currentTime - dayEnd - half_evening) * 1.f / half_evening;
            color = Utils::ColorLerpRGB(sunSetColor, nightColor, r);

		}
        directionalLight->SetColor(color);
	}
}

void CDayNightCycle::UpdateSunPosition()
{
    if (directionalLight == nullptr)
	{
		Log("Directional light not set in CDayNightCycle but is used.");
		return;
	}
    glm::vec3 current_pos = directionalLight->GetPosition();

    float m_SunAngle = currentTime * 360.f;
	float rad = (m_SunAngle)*static_cast<float>(M_PI) / 180.0f + static_cast<float>(M_PI) / 2.f;
    current_pos.x = defaultSunPos.x * cos(rad);
    current_pos.y = defaultSunPos.y * fabs(cos(rad / 2.f)) + 1000.f;
    current_pos.z = defaultSunPos.z *  sin(rad);
    directionalLight->SetPosition(current_pos);
	//Utils::PrintVector("", current_pos);
	//std::cout << m_SunAngle << std::endl;
}

void CDayNightCycle::CalculateBlendFactor()
{
	//Full Night : 0.85 - 0.15
	//Full Day :  0.35 - 0.65
	//Blend time night-day : 0.15-0.35
	//Blend time day-night : 0.65-0.85
	if (IsNight())
	{
        dayNightBlendFactor = 0.f;
	}
	else if (IsDay())
	{
        dayNightBlendFactor = 1.f;
	}
	else if (IsMorning())
	{
        dayNightBlendFactor = (currentTime - nightEnd) * 1.f / morningDuration;
	}
	else if (IsEvening())
	{
        dayNightBlendFactor = 1.f - (currentTime - dayEnd) * 1.f / eveningDuration;
	}
	else
	{
        dayNightBlendFactor = 0.f;
	}
}

void CDayNightCycle::SetDirectionalLight(CLight * light)
{
    defaultSunPos = light->GetPosition();
    directionalLight = light;
}

const void CDayNightCycle::GetCurrentHour(int & hour, int & minutes) const
{
	hour = static_cast<int>(GetHours());

    minutes = static_cast<int>(currentTime*24.f*60.f - (hour * 60.f));
	return void();
}

const bool CDayNightCycle::IsDay() const
{
    if (currentTime > dayStart && currentTime < dayEnd)
		return true;
	return false;
}

const bool CDayNightCycle::IsNight() const
{
    if (currentTime > nightStart || currentTime < nightEnd)
		return true;
	return false;
}

const bool CDayNightCycle::IsMorning() const
{
    if (currentTime > nightEnd && currentTime < dayStart)
		return true;
	return false;
}

const bool CDayNightCycle::IsEvening() const
{
    if (currentTime > dayEnd && currentTime < nightStart)
		return true;
	return false;
}

const bool CDayNightCycle::IsFirstHalfMorning() const
{
    if (currentTime > nightEnd && currentTime < (nightEnd + morningDuration/2.f))
		return true;
	return false;
}

const bool CDayNightCycle::IsFirstHalfEvening() const
{
    if (currentTime > dayEnd && currentTime < (dayEnd + eveningDuration / 2.f))
		return true;
	return false;
}
