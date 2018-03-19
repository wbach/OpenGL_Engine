#include "DayNightCycle.h"
#include "GLM/GLMUtils.h"
#include "Logger/Log.h"


CDayNightCycle::CDayNightCycle()
    : CGameTime()
    , directionalLight(nullptr)
    , sunRiseColor(Utils::RGBtoFloat(253.f, 168.f, 87.f))
    , midDayColor(1.f, 0.784314f, 0.588235)
    , sunSetColor(Utils::RGBtoFloat(253.f, 94.f, 83.f))
    , nightColor(vec3(.01f))
    , dayNightBlendFactor(0.5f)
    , dayStart(.45f)
    , dayEnd(.75f)
    , nightStart(.95f)
    , nightEnd(0.25f)
{
    morningDuration = dayStart - nightEnd;
    eveningDuration = nightStart - dayEnd;
    defaultSunPos = vec3(10000, 15000, 10000);
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
		vec3 color;
        float half_moorning_duration = morningDuration / 2.f;
		if (IsFirstHalfMorning())
		{
            float r = (currentTime_ - nightEnd) * 1.f / half_moorning_duration;
            color = Utils::ColorLerpRGB(nightColor, sunRiseColor, r);
		}
		else
		{
            float r = (currentTime_ - nightEnd - half_moorning_duration) * 1.f / half_moorning_duration;
            color = Utils::ColorLerpRGB(sunRiseColor, midDayColor, r);
		}
        directionalLight->SetColor(color);
	}
	if (IsEvening())
	{
		vec3 color;
        float half_evening = eveningDuration / 2.f;
		if (IsFirstHalfEvening())
		{
            float r = (currentTime_ - dayEnd) * 1.f / half_evening;
            color = Utils::ColorLerpRGB(midDayColor, sunSetColor, r);
		}
		else
		{
            float r = (currentTime_ - dayEnd - half_evening) * 1.f / half_evening;
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
    vec3 current_pos = directionalLight->GetPosition();

    float m_SunAngle = currentTime_ * 360.f;
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
        dayNightBlendFactor = (currentTime_ - nightEnd) * 1.f / morningDuration;
	}
	else if (IsEvening())
	{
        dayNightBlendFactor = 1.f - (currentTime_ - dayEnd) * 1.f / eveningDuration;
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

const wb::vec2i CDayNightCycle::GetCurrentHour() const
{
	int hour = static_cast<int>(GetHours());

    int minutes = static_cast<int>(currentTime_*24.f*60.f - (hour * 60.f));

	return wb::vec2i(hour, minutes);
}

const bool CDayNightCycle::IsDay() const
{
    return (currentTime_ > dayStart && currentTime_ < dayEnd);
}

const bool CDayNightCycle::IsNight() const
{
    return (currentTime_ > nightStart || currentTime_ < nightEnd);
}

const bool CDayNightCycle::IsMorning() const
{
    return (currentTime_ > nightEnd && currentTime_ < dayStart);
}

const bool CDayNightCycle::IsEvening() const
{
    return (currentTime_ > dayEnd && currentTime_ < nightStart);
}

const bool CDayNightCycle::IsFirstHalfMorning() const
{
    return (currentTime_ > nightEnd && currentTime_ < (nightEnd + morningDuration/2.f));
}

const bool CDayNightCycle::IsFirstHalfEvening() const
{
    return (currentTime_ > dayEnd && currentTime_ < (dayEnd + eveningDuration / 2.f));
}
