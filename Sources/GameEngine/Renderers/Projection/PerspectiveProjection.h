#pragma once
#include <Types.h>

#include <optional>

#include "Projection.h"

namespace GameEngine
{
class PerspectiveProjection : public Projection
{
public:
    PerspectiveProjection();
    PerspectiveProjection(const vec2ui& renderingSize);
    PerspectiveProjection(const vec2ui& renderingSize, float near, float far, float fov);

    PerspectiveProjection(const PerspectiveProjection& p);
    ~PerspectiveProjection();

    PerspectiveProjection& operator=(const PerspectiveProjection& p);

    void UpdateMatrix() override;
    float GetFoV() const;

private:
    float fov_;
};
}  // namespace GameEngine
