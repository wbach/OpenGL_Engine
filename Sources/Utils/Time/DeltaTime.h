#pragma once
#include <Types.h>

namespace Utils
{
namespace Time
{
class DeltaTime
{
public:
    static constexpr double NANOSECONDS_PER_SECOND = 1'000'000'000.0;
    constexpr DeltaTime()                          = default;

    explicit DeltaTime(int64 nanoseconds) noexcept
    {
        *this = nanoseconds;
    }

    DeltaTime& operator=(int64 nanoseconds) noexcept
    {
        deltaTime_ = nanoseconds;

        const double seconds = static_cast<double>(nanoseconds) / NANOSECONDS_PER_SECOND;

        deltaTimeDouble_ = seconds;
        deltaTimeFloat_  = static_cast<float>(seconds);
        return *this;
    }

    int64 GetNanoseconds() const noexcept
    {
        return deltaTime_;
    }

    double GetSeconds() const noexcept
    {
        return deltaTimeDouble_;
    }

    float GetSecondsFloat() const noexcept
    {
        return deltaTimeFloat_;
    }

    float operator*(float value) const noexcept
    {
        return deltaTimeFloat_ * value;
    }

    double operator*(double value) const noexcept
    {
        return deltaTimeDouble_ * value;
    }

    friend float operator*(float value, const DeltaTime& dt) noexcept
    {
        return dt * value;
    }

    friend double operator*(double value, const DeltaTime& dt) noexcept
    {
        return dt * value;
    }

private:
    int64 deltaTime_{0};           // Nanoseconds
    double deltaTimeDouble_{0.0};  // Seconds
    float deltaTimeFloat_{0.f};    // Seconds
};
}  // namespace Time
}  // namespace Utils
