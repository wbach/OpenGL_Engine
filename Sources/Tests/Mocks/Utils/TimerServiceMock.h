#pragma once

#include <Time/ITimerService.h>
#include <gmock/gmock.h>

namespace Utils
{
namespace Time
{
class TimerServiceMock : public ITimerService
{
public:
    MOCK_METHOD(IdType, timer, (const std::chrono::milliseconds&, std::function<void()>), (override));
    MOCK_METHOD(IdType, periodicTimer, (const std::chrono::milliseconds&, std::function<void()>), (override));
    MOCK_METHOD(void, cancel, (IdType), (override));
};
}  // namespace Time
}  // namespace Utils
