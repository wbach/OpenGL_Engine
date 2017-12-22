#pragma once
#include <math.h>

class CGameTime
{
public:
    virtual void Update(const float& delta_time);
    ~CGameTime(){}
    float GetHours() const;
    float GetMinutes() const;
    float GetSeconds() const;
    float GetTime() const;
    float GetDeltaTime() const;
    float GetDeltaGameTime() const;
    void SetTime(const float& time);

protected:
    float currentTime   = 0.12f;
    float deltaTime     = 0.f;
    float deltaGameTime = 0.f;
};

inline float CGameTime::GetHours() const
{
    return currentTime * 24.f;
}
inline float CGameTime::GetMinutes() const
{
    return currentTime * 24.f * 60.f;
}
inline float CGameTime::GetSeconds() const
{
    return currentTime * 24.f * 60.f * 60.f;
}
inline float CGameTime::GetTime() const
{
    return currentTime;
}
inline float CGameTime::GetDeltaTime() const
{
    return deltaTime;
}
inline float CGameTime::GetDeltaGameTime() const
{
    return deltaGameTime;
}
inline void CGameTime::SetTime(const float& time)
{
    currentTime = time;
}
