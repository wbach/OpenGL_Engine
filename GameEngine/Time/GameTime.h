#pragma once
#include <math.h>
class CGameTime
{
public:
	virtual void Update(const float& delta_time);
	float GetHours() const;
	float GetMinutes() const;
	float GetSeconds() const;
	float GetTime() const;
	float GetDeltaTime() const;
	float GetDeltaGameTime() const;
	void SetTime(const float& time);
protected:
	float m_CurrentTime = 0.12f;
	float m_DeltaTime = 0.f;
	float m_DeltaGameTime = 0.f;
};
inline float CGameTime::GetHours() const
{
	return m_CurrentTime*24.f;
}
inline float CGameTime::GetMinutes() const
{
	return m_CurrentTime*24.f*60.f;
}
inline float CGameTime::GetSeconds() const
{
	return m_CurrentTime*24.f*60.f*60.f;
}
inline float CGameTime::GetTime() const
{
	return m_CurrentTime;
}
inline float CGameTime::GetDeltaTime() const
{
	return m_DeltaTime;
}
inline float CGameTime::GetDeltaGameTime() const
{
	return m_DeltaGameTime;
}
inline void CGameTime::SetTime(const float& time)
{
	m_CurrentTime = time;
}