#pragma once
#include <math.h>

#include <ostream>

namespace GameEngine
{
inline float CalculateTime(int hour, int minute)
{
    return (hour + minute / 60.0f) / 24.0f;
}

class GameTime
{
public:
    struct HourMinuteSecond
    {
        int hour{0};
        int minute{0};
        int second{0};
    };

    using DeltaTime = float;
    using Time      = float;

    GameTime();
    virtual ~GameTime() = default;

    virtual void Update(DeltaTime);
    virtual void SetTime(Time time);

    float GetHours() const;
    float GetMinutes() const;
    float GetSeconds() const;
    float GetTime() const;
    float GetDeltaTime() const;
    float GetDeltaGameTime() const;
    HourMinuteSecond GetHourMinuteSecond() const;

    void SetHourMinuteSecond(const HourMinuteSecond& hms);
    void SetHour(float hours);
    void SetMinute(float minutes);
    void SetSecond(float seconds);
    void SetDayLengthInSeconds(float seconds);

private:
    float WrapNormalized(float);

protected:
    Time currentTime_;
    DeltaTime deltaTime_;
    DeltaTime deltaGameTime_;
    float dayLengthSeconds_;
};

std::ostream& operator<<(std::ostream&, const GameTime&);
std::ostream& operator<<(std::ostream&, const GameTime::HourMinuteSecond&);
}  // namespace GameEngine
