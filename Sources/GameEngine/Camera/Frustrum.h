#pragma once
#include "Plane.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
#include <array>
#include <list>

namespace GameEngine
{
enum class Halfspace
{
    NEGATIVE,
    ON_PLANE,
    POSITIVE
};

class Frustrum
{
public:
    Frustrum();
    void prepareFrame(const mat4& projectionViewMatrix);
    bool intersection(const vec3& point) const;
    bool intersection(const vec3& center, float radius) const;
    bool intersection(const BoundingBox&) const;
    uint64 getIntersectionsCountInFrame() const;
    void push(const mat4& projectionViewMatrix);
    void pop();

private:
    std::list<std::array<Plane, 6>> planes_;
};
}  // namespace GameEngine
