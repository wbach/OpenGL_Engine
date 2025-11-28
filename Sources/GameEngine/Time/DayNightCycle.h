#pragma once
#include "GameTime.h"
#include "Types.h"

namespace GameEngine
{
namespace Components
{
class DirectionalLightComponent;
}

class DayNightCycle : public GameTime
{
public:
    DayNightCycle();
    void Update(GameTime::DeltaTime) override;
    void SetTime(GameTime::Time) override;

    void UpdateSunLight();
    void UpdateSunColor();
    void UpdateSunPosition();
    void CalculateBlendFactor();
    void SetDirectionalLight(Components::DirectionalLightComponent*);

    const vec2i GetCurrentHour() const;
    bool IsDay() const;
    bool IsNight() const;
    bool IsMorning() const;
    bool IsEvening() const;
    bool IsFirstHalfMorning() const;
    bool IsFirstHalfEvening() const;
    float GetDayNightBlendFactor() const;

private:
    Components::DirectionalLightComponent* directionalLight;
    vec3 sunRiseColor;
    vec3 midDayColor;
    vec3 sunSetColor;
    vec3 nightColor;

    float dayNightBlendFactor;
    float dayStart;
    float dayEnd;
    float nightStart;
    float nightEnd;
    float morningDuration;
    float eveningDuration;

    vec3 defaultSunPos;
};

inline float DayNightCycle::GetDayNightBlendFactor() const
{
    return dayNightBlendFactor;
}

}  // namespace GameEngine
