#pragma once
#include "IBrush.h"
#include "Interpolation.h"
#include "LocalBrushSize.h"
#include "WorldSpaceBrushRadius.h"

namespace GameEngine
{
class CircleBrush : public IBrush
{
public:
    CircleBrush(Interpolation, WorldSpaceBrushRadius, float);

    void createInfluance(bool isEvenTextureSize, float stepX, float stepZ) override;
    const std::vector<Influance>& getInfluence() const override;
    const LocalBrushSize& getLocalBrushSize() const;

private:
    void calculateInfluence(bool isEven);

private:
    std::vector<Influance> influance;
    WorldSpaceBrushRadius worldSpaceBrushRadius;
    LocalBrushSize localBrushSize;
    Interpolation interpolation;
    vec2 steps{-1.f, -1.f};
    float strength{1.f};
};
}  // namespace GameEngine
