#pragma once
#include <Types.h>

#include "Projection.h"

namespace GameEngine
{
class OrthographicProjection : public Projection
{
public:
    using Projection::Projection;

    void UpdateMatrix() override;
    void SetZoom(float );
    float IncreaseZoom(float);
    float DecreaseZoom(float);

private:
    float zoom{1.f};
};
}  // namespace GameEngine
