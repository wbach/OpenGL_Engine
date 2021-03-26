#pragma once
#include "Types.h"

namespace GameEngine
{

class BoundingBox
{
public:
    enum NumericLimits
    {
        MaxOppositeValues
    };

    BoundingBox() = default;
    BoundingBox(NumericLimits);
    BoundingBox(const vec3&, const vec3&);
    void minMax(const vec3&, const vec3&);
    void minY(float);
    void maxY(float);
    void min(const vec3&);
    void max(const vec3&);
    void scale(const vec3&);
    void translate(const vec3&);
    const vec3& min() const;
    const vec3& max() const;
    const vec3& center() const;
    const vec3& size() const;
    float maxScale() const;

private:
    void calculate();

private:
    vec3 min_{0.f};
    vec3 max_{1.f};
    vec3 center_{0.5f};
    vec3 size_{1.f};
    float maxScale_{1.f};
};
}  // namespace GameEngine
