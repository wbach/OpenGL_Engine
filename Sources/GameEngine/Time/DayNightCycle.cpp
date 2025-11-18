#include "DayNightCycle.h"

#include "GLM/GLMUtils.h"
#include "GameEngine/Time/GameTime.h"
#include "Logger/Log.h"

namespace GameEngine
{
DayNightCycle::DayNightCycle(Light* directionalLight)
    : GameTime()
    , directionalLight(directionalLight)
    , sunRiseColor(Utils::RGBtoFloat(253.f, 168.f, 87.f))
    , midDayColor(1.f, 0.784314f, 0.588235)
    , sunSetColor(Utils::RGBtoFloat(253.f, 94.f, 83.f))
    , nightColor(vec3(0.1, 0.1, 0.2f))
    , dayNightBlendFactor(0.5f)
    , dayStart(CalculateTime(9, 00))
    , dayEnd(CalculateTime(18, 00))
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
    return (t >= a || t <= b); // wrap-around przez zero
}

void DayNightCycle::UpdateSunPosition()
{
    if (!directionalLight)
        return;

    float radius = 15000.f;
    glm::vec3 pos;
    float angle;

    float offset = 0.f;// CalculateTime(1, 0);

    float nightStartO = fmod(nightStart + offset, 1.f);
    float nightEndO   = fmod(nightEnd   - offset + 1.f, 1.f);

    bool night = inRange(currentTime_, nightStartO, nightEndO);

    if (!night)
    {
        // dzień (poranek + południe + wieczór)
        float length = (nightStartO - nightEndO + 1.f);
        if (length > 1.f) length -= 1.f;

        float dayProgress;
        if (currentTime_ >= nightEndO)
            dayProgress = (currentTime_ - nightEndO) / length;
        else
            dayProgress = (currentTime_ + 1.f - nightEndO) / length;

        angle = glm::pi<float>() * dayProgress;

        pos.x = radius * cos(angle);
        pos.z = radius * sin(angle);
        pos.y = radius * sin(angle);

        directionalLight->SetPosition(pos);
    }
    else
    {
        // noc (księżyc)
        float length = (nightEndO - nightStartO + 1.f);
        if (length > 1.f) length -= 1.f;

        float nightProgress;
        if (currentTime_ >= nightStartO)
            nightProgress = (currentTime_ - nightStartO) / length;
        else
            nightProgress = (currentTime_ + 1.f - nightStartO) / length;

        angle = glm::pi<float>() * (1.0f - nightProgress);

        pos.x = radius * cos(angle);
        pos.z = radius * sin(angle);
        pos.y = radius * sin(angle);

        directionalLight->SetPosition(pos);
    }

    LOG_DEBUG << "directionalLight dir : " << directionalLight->GetDirection();
}

void DayNightCycle::CalculateBlendFactor()
{
    // Full Night : 0.85 - 0.15
    // Full Day :  0.35 - 0.65
    // Blend time night-day : 0.15-0.35
    // Blend time day-night : 0.65-0.85
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
    return (currentTime_ > dayStart && currentTime_ < dayEnd);
}

bool DayNightCycle::IsNight() const
{
    return (currentTime_ > nightStart || currentTime_ < nightEnd);
}

bool DayNightCycle::IsMorning() const
{
    return (currentTime_ > nightEnd && currentTime_ < dayStart);
}

bool DayNightCycle::IsEvening() const
{
    return (currentTime_ > dayEnd && currentTime_ < nightStart);
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
