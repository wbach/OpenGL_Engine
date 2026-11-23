#include "GameTime.h"

#include <Logger/Log.h>

namespace GameEngine
{
GameTime::GameTime()
    : currentTime_(0.5f)
    , deltaTime_(0.f)
    , deltaGameTime_(0.f)
    , dayLengthSeconds_{600.f}
{
}

void GameTime::Update(DeltaTime dt)
{
    const auto dtSeconds     = dt / 1000.0f;
    const auto cycleProgress = dtSeconds / dayLengthSeconds_;

    currentTime_ = WrapNormalized(currentTime_ + cycleProgress);

    deltaGameTime_ = dtSeconds;
    deltaTime_     = dt;
}
void GameTime::SetTime(Time time)
{
    currentTime_ = time;
}
float GameTime::GetHours() const
{
    return currentTime_ * 24.f;
}
float GameTime::GetMinutes() const
{
    return currentTime_ * 24.f * 60.f;
}
float GameTime::GetSeconds() const
{
    return currentTime_ * 24.f * 60.f * 60.f;
}
float GameTime::GetTime() const
{
    return currentTime_;
}
float GameTime::GetDeltaTime() const
{
    return deltaTime_;
}
float GameTime::GetDeltaGameTime() const
{
    return deltaGameTime_;
}
GameTime::HourMinuteSecond GameTime::GetHourMinuteSecond() const
{
    auto totalSeconds = static_cast<int>(GetSeconds());
    auto hour         = totalSeconds / 3600;
    auto minute       = (totalSeconds % 3600) / 60;
    auto second       = totalSeconds % 60;
    return {hour, minute, second};
}
void GameTime::SetHourMinuteSecond(const HourMinuteSecond& hms)
{
    auto totalSeconds = hms.hour * 3600 + hms.minute * 60 + hms.second;
    currentTime_     = static_cast<Time>(totalSeconds) / (24.0f * 3600.0f);
}
void GameTime::SetHour(float hours)
{
    auto v = GetHourMinuteSecond();
    v.hour = hours;
    SetHourMinuteSecond(v);
}
void GameTime::SetMinute(float minutes)
{
    auto v   = GetHourMinuteSecond();
    v.minute = minutes;
    SetHourMinuteSecond(v);
}
void GameTime::SetSecond(float seconds)
{
    auto v   = GetHourMinuteSecond();
    v.second = seconds;
    SetHourMinuteSecond(v);
}
void GameTime::SetDayLengthInSeconds(float seconds)
{
    dayLengthSeconds_ = seconds;
}
float GameTime::WrapNormalized(float t)
{
    t = std::fmod(t, 1.0f);
    if (t < 0.0f)
    {
        t += 1.0f;
    }
    return t;
}
std::ostream& operator<<(std::ostream& os, const GameTime& gameTime)
{
    const auto& hms = gameTime.GetHourMinuteSecond();
    os << "Current time: " << hms;
    return os;
}
std::ostream& operator<<(std::ostream& os, const GameTime::HourMinuteSecond& hms)
{
    os << "Current time: " << (hms.hour < 10 ? "0" : "") << hms.hour << ":" << (hms.minute < 10 ? "0" : "") << hms.minute << ":"
       << (hms.second < 10 ? "0" : "") << hms.second;
    return os;
}
}  // namespace GameEngine
