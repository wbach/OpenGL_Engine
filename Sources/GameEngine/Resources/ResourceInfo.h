#pragma once
#include <Types.h>
#include <memory>

namespace GameEngine
{
enum class ResourceGpuStatus
{
    NotLoaded,
    Loaded
};

template <class T>
struct ResourceInfo
{
    uint32 instances_{1};
    ResourceGpuStatus resourceGpuStatus_{ResourceGpuStatus::NotLoaded};
    std::unique_ptr<T> resource_{nullptr};
};
} // namespace GameEngine
