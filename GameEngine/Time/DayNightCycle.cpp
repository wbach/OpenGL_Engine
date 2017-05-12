#include "DayNightCycle.h"
#include "../Utils/GLM/GLMUtils.h"
#include "../Debug_/Log.h"
#include <GL/glew.h>

CDayNightCycle::CDayNightCycle()
	: m_DirectionalLight(nullptr)
	, m_SunRiseColor(Utils::RGBtoFloat(253.f, 168.f, 87.f))
	, m_MidDayColor(1.f, 0.784314f, 0.588235)
	, m_SunSetColor(Utils::RGBtoFloat(253.f, 94.f, 83.f))
	, m_NightColor(glm::vec3(.01f))
	, m_DayNightBlendFactor(0.5f)
	, m_DayStart(.45f)
	, m_DayEnd(.75f)
	, m_NightStart(.95f)
	, m_NightEnd(0.25f)
	, CGameTime()
{
	m_MorningDuration = m_DayStart - m_NightEnd;
	m_EveningDuration = m_NightStart - m_DayEnd;
	m_DefaultSunPos = glm::vec3(10000, 15000, 10000);
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
	if (m_DirectionalLight == nullptr)
	{
		Log("Directional light not set in CDayNightCycle but is used.");
		return;
	}
	if (IsNight())
	{
		m_DirectionalLight->SetColor(m_NightColor);
	}
	if (IsDay())
	{
		m_DirectionalLight->SetColor(m_MidDayColor);
	}
	if (IsMorning())
	{
		glm::vec3 color;
		float half_moorning_duration = m_MorningDuration / 2.f;
		if (IsFirstHalfMorning())
		{
			float r = (m_CurrentTime - m_NightEnd) * 1.f / half_moorning_duration;
			color = Utils::ColorLerpRGB(m_NightColor, m_SunRiseColor, r);
		}
		else
		{
			float r = (m_CurrentTime - m_NightEnd - half_moorning_duration) * 1.f / half_moorning_duration;
			color = Utils::ColorLerpRGB(m_SunRiseColor, m_MidDayColor, r);			
		}
		m_DirectionalLight->SetColor(color);
	}
	if (IsEvening())
	{
		glm::vec3 color;
		float half_evening = m_EveningDuration / 2.f;
		if (IsFirstHalfEvening())
		{
			float r = (m_CurrentTime - m_DayEnd) * 1.f / half_evening;
			color = Utils::ColorLerpRGB(m_MidDayColor, m_SunSetColor, r);
		}
		else
		{
			float r = (m_CurrentTime - m_DayEnd - half_evening) * 1.f / half_evening;
			color = Utils::ColorLerpRGB(m_SunSetColor, m_NightColor, r);

		}
		m_DirectionalLight->SetColor(color);
	}
}

void CDayNightCycle::UpdateSunPosition()
{
	if (m_DirectionalLight == nullptr)
	{
		Log("Directional light not set in CDayNightCycle but is used.");
		return;
	}
	glm::vec3 current_pos = m_DirectionalLight->GetPosition();

	float m_SunAngle = m_CurrentTime * 360.f;
	float rad = (m_SunAngle)*static_cast<float>(M_PI) / 180.0f + static_cast<float>(M_PI) / 2.f;
	current_pos.x = m_DefaultSunPos.x * cos(rad);
    current_pos.y = m_DefaultSunPos.y * fabs(cos(rad / 2.f)) + 1000.f;
	current_pos.z = m_DefaultSunPos.z *  sin(rad);
	m_DirectionalLight->SetPosition(current_pos);
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
		m_DayNightBlendFactor = 0.f;
	}
	else if (IsDay())
	{
		m_DayNightBlendFactor = 1.f;
	}
	else if (IsMorning())
	{
		m_DayNightBlendFactor = (m_CurrentTime - m_NightEnd) * 1.f / m_MorningDuration;
	}
	else if (IsEvening())
	{
		m_DayNightBlendFactor = 1.f - (m_CurrentTime - m_DayEnd) * 1.f / m_EveningDuration;
	}
	else
	{
		m_DayNightBlendFactor = 0.f;
	}
}

void CDayNightCycle::SetDirectionalLight(CLight * light)
{
    m_DefaultSunPos = light->GetPosition();
	m_DirectionalLight = light;
}

const void CDayNightCycle::GetCurrentHour(int & hour, int & minutes) const
{
	hour = static_cast<int>(GetHours());

	minutes = static_cast<int>(m_CurrentTime*24.f*60.f - (hour * 60.f));
	return void();
}

const bool CDayNightCycle::IsDay() const
{
	if (m_CurrentTime > m_DayStart && m_CurrentTime < m_DayEnd)
		return true;
	return false;
}

const bool CDayNightCycle::IsNight() const
{
	if (m_CurrentTime > m_NightStart || m_CurrentTime < m_NightEnd)
		return true;
	return false;
}

const bool CDayNightCycle::IsMorning() const
{
	if (m_CurrentTime > m_NightEnd && m_CurrentTime < m_DayStart)
		return true;
	return false;
}

const bool CDayNightCycle::IsEvening() const
{
	if (m_CurrentTime > m_DayEnd && m_CurrentTime < m_NightStart)
		return true;
	return false;
}

const bool CDayNightCycle::IsFirstHalfMorning() const
{
	if (m_CurrentTime > m_NightEnd && m_CurrentTime < (m_NightEnd + m_MorningDuration/2.f))
		return true;
	return false;
}

const bool CDayNightCycle::IsFirstHalfEvening() const
{
	if (m_CurrentTime > m_DayEnd && m_CurrentTime < (m_DayEnd + m_EveningDuration / 2.f))
		return true;
	return false;
}
