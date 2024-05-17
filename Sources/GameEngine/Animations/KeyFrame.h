#pragma once
#include <unordered_map>

#include "JointTransform.h"

namespace GameEngine
{
namespace Animation
{
using JointId = uint32;

struct TimeStamp
{
    float value;

    TimeStamp& operator=(float b)
    {
        value = b;
        return *this;
    }

    TimeStamp& operator=(const TimeStamp& b)
    {
        value = b.value;
        return *this;
    }

    bool operator==(float v) const
    {
        return value == v;
    }
    bool operator==(const TimeStamp& v) const
    {
        return value == v.value;
    }

    bool operator!=(float v) const
    {
        return value != v;
    }
    bool operator!=(const TimeStamp& v) const
    {
        return value != v.value;
    }

    bool operator<(float a) const
    {
        return value < a;
    }
    bool operator<(const TimeStamp& a) const
    {
        return value < a.value;
    }

    bool operator>(const TimeStamp& a) const
    {
        return value > a.value;
    }
    bool operator>(float a) const
    {
        return value > a;
    }
};

struct KeyFrame
{
    TimeStamp timeStamp{0.f};
    std::unordered_map<JointId, JointTransform> transforms;
};
}  // namespace Animation
}  // namespace GameEngine
