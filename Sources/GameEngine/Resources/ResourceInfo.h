#pragma once
#include <Types.h>
#include <memory>

namespace GameEngine
{
template <class T>
struct ResourceInfo
{
    uint32 instances_{1};
    std::unique_ptr<T> resource_{nullptr};
};
} // namespace GameEngine