#pragma once
#include <vector>
#include "Types.h"

namespace Utils
{
class Beizer
{
public:
    Beizer(const std::vector<vec2>& controlPoints, float timeStep = 0.01f);
    vec2 getValue(float t) const;
    vec2 calculateValue(float t) const;

private:
    bool isValidateTime(float t) const;

private:
    std::vector<vec2> controlPoints_;
    std::vector<vec2> preCalculatedPoints_;
    float defaultTimeStep_;
};
}  // namespace Utils
