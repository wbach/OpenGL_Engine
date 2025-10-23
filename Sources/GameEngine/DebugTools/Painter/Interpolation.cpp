#include "Interpolation.h"

#include "Types.h"
#include <algorithm>

namespace GameEngine
{
Interpolation makeInterpolation(InterpolationType type)
{
    switch (type)
    {
        case InterpolationType::Hard:
            return [](const vec2i& point, const vec2i& center, uint32 brushSize) -> float
            {
                if (brushSize == 0)
                    return 1.0f;

                vec2 offset{static_cast<float>(point.x) - center.x, static_cast<float>(point.y) - static_cast<float>(center.y)};
                float dist = std::sqrt(offset.x * offset.x + offset.y * offset.y);
                return dist <= static_cast<float>(brushSize) ? 1.0f : 0.0f;
            };

        case InterpolationType::Linear:
            return [](const vec2i& point, const vec2i& center, uint32 brushSize) -> float
            {
                if (brushSize == 0)
                    return 1.0f;

                vec2 offset{static_cast<float>(point.x) - static_cast<float>(center.x),
                            static_cast<float>(point.y) - static_cast<float>(center.y)};
                float dist = std::sqrt(offset.x * offset.x + offset.y * offset.y);
                float t    = std::clamp(dist / static_cast<float>(brushSize), 0.0f, 1.0f);
                return 1.0f - t;
            };

        case InterpolationType::Smooth:
            return [](const vec2i& point, const vec2i& center, uint32 brushSize) -> float
            {
                if (brushSize == 0)
                    return 1.0f;
                vec2 offset{static_cast<float>(point.x) - center.x, static_cast<float>(point.y) - static_cast<float>(center.y)};
                float dist = std::sqrt(offset.x * offset.x + offset.y * offset.y);
                float t    = std::clamp(dist / static_cast<float>(brushSize), 0.0f, 1.0f);
                t          = t * t * (3.0f - 2.0f * t);  // smoothstep
                return 1.0f - t;
            };

        case InterpolationType::Gaussian:
            return [](const vec2i& point, const vec2i& center, uint32 brushSize) -> float
            {
                if (brushSize == 0)
                    return 1.0f;

                vec2 offset{static_cast<float>(point.x) - center.x, static_cast<float>(point.y) - static_cast<float>(center.y)};
                float dist  = std::sqrt(offset.x * offset.x + offset.y * offset.y);
                float sigma = static_cast<float>(brushSize) / 2.0f;
                return std::exp(-(dist * dist) / (2.0f * sigma * sigma));
            };
        case InterpolationType::ConstValue:
        default:
            return [](const vec2i&, const vec2i&, uint32) -> float { return 1.f; };
            break;
    }
}
}  // namespace GameEngine