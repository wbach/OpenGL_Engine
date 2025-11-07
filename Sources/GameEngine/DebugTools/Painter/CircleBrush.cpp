#include "CircleBrush.h"

#include <Logger/Log.h>

#include "PainterUtils.h"

namespace GameEngine
{
CircleBrush::CircleBrush(Interpolation interpolation, WorldSpaceBrushRadius size, float strength)
    : worldSpaceBrushRadius(size)
    , localBrushSize{static_cast<uint32>(std::round(size.value))}
    , interpolation{interpolation}
    , strength(strength)
{
    calculateInfluence(false);
}
void CircleBrush::createInfluance(bool isEven, float stepX, float stepZ)
{
    const float epsilon = 0.0001f;
    glm::vec2 newSteps(stepX, stepZ);

    if (glm::all(glm::epsilonEqual(newSteps, steps, epsilon)))
    {
        LOG_DEBUG << "Influance vector not change skip";
        return;
    }

    steps = newSteps;

    const double brushSizeX     = worldSpaceBrushRadius.value / stepX;
    const double brushSizeZ     = worldSpaceBrushRadius.value / stepZ;
    const double brushSizeInMap = std::min(brushSizeX, brushSizeZ);

    localBrushSize = LocalBrushSize{static_cast<uint32>(std::max(1.0, std::round(brushSizeInMap)))};

    calculateInfluence(isEven);
}
const std::vector<Influance>& CircleBrush::getInfluence() const
{
    return influance;
}
const LocalBrushSize& CircleBrush::getLocalBrushSize() const
{
    return localBrushSize;
}

void CircleBrush::calculateInfluence(bool isEven)
{
    influance.clear();
    int32 brushSize = static_cast<int32>(localBrushSize.value);

    const glm::vec2 centerOffset = isEven ? glm::vec2(0.5f, 0.5f) : glm::vec2(0.0f);
    const glm::vec2 centerPoint  = centerOffset;

    float effectiveRadius = static_cast<float>(brushSize) + (isEven ? 0.5f : 0.0f);

    const int32 limit = isEven ? brushSize : brushSize + 1;

    for (int32 y = -limit; y < limit; ++y)
    {
        for (int32 x = -limit; x < limit; ++x)
        {
            glm::vec2 samplePoint = glm::vec2(x, y) + centerOffset;
            if (glm::dot(samplePoint, samplePoint) <= effectiveRadius * effectiveRadius)
            {
                float intensity = interpolation(samplePoint, centerPoint, localBrushSize.value) * strength;
                if (fabsf(intensity) < 1e-6f)
                    continue;
                influance.push_back({.point = vec2i{x, y}, .influance = intensity});
            }
        }
    }
    sort(influance);
}
const WorldSpaceBrushRadius& CircleBrush::getWorldSpaceBrushRadius() const
{
    return worldSpaceBrushRadius;
}
}  // namespace GameEngine
