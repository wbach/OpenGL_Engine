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
};
}  // namespace GameEngine
