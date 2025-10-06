#pragma once
#include "Plane.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
#include <array>
#include <list>
#include <vector>

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
    const std::vector<std::array<Plane, 6>>& getPlanes() const;

private:
    std::vector<std::array<Plane, 6>> planes_;
    mat4 projectionViewMatrix;
};
}  // namespace GameEngine
