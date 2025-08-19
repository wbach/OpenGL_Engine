#pragma once
#include <math.h>

namespace GameEngine
{
class GameTime
{
public:
    struct HourMinuteSecond
    {
        int hour;
        int minute;
        int second;
    };

    using DeltaTime = float;
    using Time = float;

    GameTime();
    virtual ~GameTime() = default;

    virtual void Update(DeltaTime);
    virtual void SetTime(Time);

    float GetHours() const;
    float GetMinutes() const;
    float GetSeconds() const;
    float GetTime() const;
    float GetDeltaTime() const;
    float GetDeltaGameTime() const;
    HourMinuteSecond GetHourMinuteSecond() const;

protected:
    Time currentTime_;
    DeltaTime deltaTime_;
    DeltaTime deltaGameTime_;
};

inline GameTime::HourMinuteSecond GameTime::GetHourMinuteSecond() const
{
    int totalSeconds = static_cast<int>(GetSeconds());
    int hour   = totalSeconds / 3600;
    int minute = (totalSeconds % 3600) / 60;
    int second = totalSeconds % 60;
    return {hour, minute, second};
}
inline float GameTime::GetHours() const
{
    return currentTime_ * 24.f;
}
inline float GameTime::GetMinutes() const
{
    return currentTime_ * 24.f * 60.f;
}
inline float GameTime::GetSeconds() const
{
    return currentTime_ * 24.f * 60.f * 60.f;
}
inline float GameTime::GetTime() const
{
    return currentTime_;
}
inline float GameTime::GetDeltaTime() const
{
    return deltaTime_;
}
inline float GameTime::GetDeltaGameTime() const
{
    return deltaGameTime_;
}
inline void GameTime::SetTime(GameTime::Time time)
{
    currentTime_ = time;
}
}  // namespace GameEngine
