#pragma once

#include <chrono>
#include <functional>

#include "EngineApi.h"

namespace Utils
{
namespace Time
{
struct ENGINE_API Timer
{
    std::chrono::steady_clock::time_point expiry;
    std::chrono::milliseconds interval;
    std::function<void()> callback;
    bool periodic;
};
}  // namespace Time
}  // namespace Utils
