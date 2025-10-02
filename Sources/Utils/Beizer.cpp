#include <cmath>
#include "Beizer.hpp"

namespace Utils
{
int factorial(int n)
{
    if ((n == 0) || (n == 1))
    {
        return 1;
    }
    else
    {
        auto result = (n * factorial(n - 1));
        return result;
    }
}
float factorialf(int n)
{
    return static_cast<float>(factorial(n));
}
float bernstein(int i, int n, float t)
{
    float r = factorialf(n) / (factorialf(i) * factorialf(n - i));
    r *= powf(1 - t, static_cast<float>(n - i));
    r *= powf(t, static_cast<float>(i));
    return r;
}
Beizer::Beizer(const std::vector<vec2>& controlPoints, float timeStep)
    : controlPoints_(controlPoints)
    , defaultTimeStep_(timeStep)
{
    preCalculatedPoints_.reserve(static_cast<int>(1.f / timeStep) + 1);

    for (float t = 0.f; t < 1.f; t += timeStep)
    {
        preCalculatedPoints_.push_back(calculateValue(t));
    }
}

vec2 Beizer::getValue(float t) const
{
    vec2 result(0, 0);

    if (isValidateTime(t))
        return result;

    auto d = static_cast<int>(floor(t / defaultTimeStep_));
    return preCalculatedPoints_[d];
}

vec2 Beizer::calculateValue(float t) const
{
    vec2 result(0, 0);

    if (isValidateTime(t))
        return result;

    float tmp = 0;
    auto n    = static_cast<int>(controlPoints_.size());

    for (int i = 0; i < n; ++i)
    {
        float bm = bernstein(i, n, t);
        tmp += bm;
        result += bm * controlPoints_[i];
    }

    result /= tmp;

    return result;
}

bool Beizer::isValidateTime(float t) const
{
    return (t < 0.f or t > 1.f);
}
}  // namespace Utils