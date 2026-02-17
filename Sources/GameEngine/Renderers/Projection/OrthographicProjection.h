#pragma once
#include <Types.h>

#include "Projection.h"

namespace GameEngine
{
const inline float REFERENCE_ORTHO_DISTANCE = 10.f;

class OrthographicProjection : public Projection
{
public:
    using Projection::Projection;

    void UpdateMatrix() override;
    void SetZoom(float);
    float IncreaseZoom(float);
    float DecreaseZoom(float);

    ProjectionType GetType() const override;

    std::unique_ptr<IProjection> Clone() const override;

private:
    float zoom{1.f};
};
}  // namespace GameEngine
