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

    constexpr DeltaTime() = default;
    explicit DeltaTime(int64 nanoseconds) noexcept
        : deltaTime_{nanoseconds}
        , deltaTimeDouble_{static_cast<double>(nanoseconds) / NANOSECONDS_PER_SECOND}
        , deltaTimeFloat_{static_cast<float>(deltaTimeDouble_)}
    {
    }

    DeltaTime& operator=(int64 nanoseconds) noexcept
    {
        deltaTime_           = nanoseconds;
        const double seconds = static_cast<double>(nanoseconds) / NANOSECONDS_PER_SECOND;
        deltaTimeDouble_     = seconds;
        deltaTimeFloat_      = static_cast<float>(seconds);
        return *this;
    }

    DeltaTime operator*(float multiplier) const noexcept
    {
        const auto scaledNanoseconds = static_cast<int64>(std::llround(static_cast<double>(deltaTime_) * multiplier));
        return DeltaTime{scaledNanoseconds};
    }

    friend DeltaTime operator*(float multiplier, const DeltaTime& dt) noexcept
    {
        return dt * multiplier;
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

private:
    int64 deltaTime_{0};           // Nanoseconds
    double deltaTimeDouble_{0.0};  // Seconds
    float deltaTimeFloat_{0.f};    // Seconds
};
}  // namespace Time
}  // namespace Utils
