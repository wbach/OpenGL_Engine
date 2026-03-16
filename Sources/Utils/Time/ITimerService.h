#pragma once
#include "Types.h"

namespace Utils
{
namespace Time
{
class ENGINE_API ITimerService
{
public:
    virtual ~ITimerService() = default;

    virtual IdType timer(const std::chrono::milliseconds&, std::function<void()>)         = 0;
    virtual IdType periodicTimer(const std::chrono::milliseconds&, std::function<void()>) = 0;
    virtual void cancel(IdType)                                                           = 0;
};
}  // namespace Time
}  // namespace Utils
