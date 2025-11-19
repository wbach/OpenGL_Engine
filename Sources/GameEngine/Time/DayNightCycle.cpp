#include "DayNightCycle.h"

#include "GLM/GLMUtils.h"
#include "GameEngine/Time/GameTime.h"
#include "Logger/Log.h"
#include "Types.h"

namespace GameEngine
{
DayNightCycle::DayNightCycle(Light* directionalLight)
    : GameTime()
    , directionalLight(directionalLight)
    , sunRiseColor(vec3(1.000, 0.766, 0.545) * 2.f)
    , midDayColor(vec3(1.000, 0.937, 0.882) * 2.f)
    , sunSetColor(vec3(1.000, 0.720, 0.460) * 2.f)
    , nightColor(vec3(0.1, 0.1, 0.2f) * 2.f)
    , dayNightBlendFactor(0.5f)
    , dayStart(CalculateTime(7, 00))
    , dayEnd(CalculateTime(20, 00))
    , nightStart(CalculateTime(21, 00))
    , nightEnd(CalculateTime(6, 00))
{
    morningDuration = dayStart - nightEnd;
    eveningDuration = nightStart - dayEnd;
    defaultSunPos   = vec3(10000, 15000, 10000);

    CalculateBlendFactor();
    if (directionalLight)
    {
        UpdateSunColor();
        UpdateSunPosition();
    }
}
void DayNightCycle::Update(GameTime::DeltaTime delta_time)
{
    GameTime::Update(delta_time);
    CalculateBlendFactor();
    UpdateSunColor();
    UpdateSunPosition();
}

void DayNightCycle::SetTime(GameTime::Time time)
{
    GameTime::SetTime(time);
    CalculateBlendFactor();
    UpdateSunColor();
    UpdateSunPosition();
}

void DayNightCycle::UpdateSunColor()
{
    if (directionalLight == nullptr)
    {
        LOG_WARN << "Directional light not set in DayNightCycle but is used.";
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
            color   = Utils::ColorLerpRGB(nightColor, sunRiseColor, r);
        }
        else
        {
            float r = (currentTime_ - nightEnd - half_moorning_duration) * 1.f / half_moorning_duration;
            color   = Utils::ColorLerpRGB(sunRiseColor, midDayColor, r);
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
            color   = Utils::ColorLerpRGB(midDayColor, sunSetColor, r);
        }
        else
        {
            float r = (currentTime_ - dayEnd - half_evening) * 1.f / half_evening;
            color   = Utils::ColorLerpRGB(sunSetColor, nightColor, r);
        }
        directionalLight->SetColor(color);
    }
}

bool inRange(float t, float a, float b)
{
    if (a < b)
        return (t >= a && t <= b);
    return (t >= a || t <= b);  // wrap-around przez zero
}

void DayNightCycle::UpdateSunPosition()
{
    if (!directionalLight)
        return;

    const float ARC    = glm::radians(200.0f);        // 200 stopni łuk
    const float EXTRA  = ARC - glm::radians(180.0f);  // nadwyżka = 20°
    const float SHIFT  = -EXTRA * 0.5f;               // -10°
    const float radius = 15000.0f;

    glm::vec3 pos;
    float angle;

    const bool night = inRange(currentTime_, nightStart, nightEnd);

    if (!night)
    {
        float dayLength = (nightStart - nightEnd + 1.0f);
        if (dayLength > 1.0f)
            dayLength -= 1.0f;

        float dayProgress;
        if (currentTime_ >= nightEnd)
            dayProgress = (currentTime_ - nightEnd) / dayLength;
        else
            dayProgress = (currentTime_ + 1.0f - nightEnd) / dayLength;

        // SUN: angle from -10° to 190°
        angle = SHIFT + ARC * dayProgress;

        pos.x = radius * cos(angle);
        pos.z = radius * sin(angle);
        pos.y = radius * sin(angle);

        directionalLight->SetPosition(pos);
    }
    else
    {
        float nightLength = (nightEnd - nightStart + 1.0f);
        if (nightLength > 1.0f)
            nightLength -= 1.0f;

        float nightProgress;
        if (currentTime_ >= nightStart)
            nightProgress = (currentTime_ - nightStart) / nightLength;
        else
            nightProgress = (currentTime_ + 1.0f - nightStart) / nightLength;

        // MOON: reverse angle from 190° to -10°
        angle = SHIFT + ARC * (1.0f - nightProgress);

        pos.x = radius * cos(angle);
        pos.z = radius * sin(angle);
        pos.y = radius * sin(angle);

        directionalLight->SetPosition(pos);
    }
}

void DayNightCycle::CalculateBlendFactor()
{
    if (IsMorning())
    {
        dayNightBlendFactor = (currentTime_ - nightEnd) / morningDuration;
    }
    else if (IsEvening())
    {
        dayNightBlendFactor = 1.f - (currentTime_ - dayEnd) / eveningDuration;
    }
    else if (IsDay())
    {
        dayNightBlendFactor = 1.f;
    }
    else if (IsNight())
    {
        dayNightBlendFactor = 0.f;
    }
    auto h = GetHourMinuteSecond();
    LOG_DEBUG << h.hour << ":" << h.minute << ", dayNightBlendFactor: " << dayNightBlendFactor;
}

void DayNightCycle::SetDirectionalLight(Light* light)
{
    defaultSunPos    = light->GetPosition();
    directionalLight = light;
}

const vec2i DayNightCycle::GetCurrentHour() const
{
    int hour = static_cast<int>(GetHours());

    int minutes = static_cast<int>(currentTime_ * 24.f * 60.f - (hour * 60.f));

    return vec2i(hour, minutes);
}

bool DayNightCycle::IsDay() const
{
    return (currentTime_ > dayStart && currentTime_ <= dayEnd);
}

bool DayNightCycle::IsNight() const
{
    return (currentTime_ > nightStart || currentTime_ <= nightEnd);
}

bool DayNightCycle::IsMorning() const
{
    return (currentTime_ > nightEnd && currentTime_ <= dayStart);
}

bool DayNightCycle::IsEvening() const
{
    return (currentTime_ > dayEnd && currentTime_ <= nightStart);
}

bool DayNightCycle::IsFirstHalfMorning() const
{
    return (currentTime_ > nightEnd && currentTime_ < (nightEnd + morningDuration / 2.f));
}

bool DayNightCycle::IsFirstHalfEvening() const
{
    return (currentTime_ > dayEnd && currentTime_ < (dayEnd + eveningDuration / 2.f));
}
}  // namespace GameEngine
