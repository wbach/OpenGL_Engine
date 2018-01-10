#pragma once
#include <math.h>

class CGameTime
{
public:
    CGameTime()
    : currentTime_(0.12f)
    , deltaTime_(0.f)
    , deltaGameTime_(0.f)
    {

    }
    ~CGameTime(){}
    virtual void Update(const float& delta_time);
    float GetHours() const;
    float GetMinutes() const;
    float GetSeconds() const;
    float GetTime() const;
    float GetDeltaTime() const;
    float GetDeltaGameTime() const;
    void SetTime(const float& time);

protected:
    float currentTime_;
    float deltaTime_;
    float deltaGameTime_;
};

inline float CGameTime::GetHours() const
{
    return currentTime_ * 24.f;
}
inline float CGameTime::GetMinutes() const
{
    return currentTime_ * 24.f * 60.f;
}
inline float CGameTime::GetSeconds() const
{
    return currentTime_ * 24.f * 60.f * 60.f;
}
inline float CGameTime::GetTime() const
{
    return currentTime_;
}
inline float CGameTime::GetDeltaTime() const
{
    return deltaTime_;
}
inline float CGameTime::GetDeltaGameTime() const
{
    return deltaGameTime_;
}
inline void CGameTime::SetTime(const float& time)
{
    currentTime_ = time;
}
