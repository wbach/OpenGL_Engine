#include "SmoothingHeightPainter.h"

#include <Input/InputManager.h>
#include <Input/KeyCodes.h>
#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/DebugTools/Painter/HeightPainter.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "IBrush.h"

namespace GameEngine
{
namespace
{
const uint32_t SMOOTH_KERNEL_SIZE = 5;
}

SmoothingHeightPainter::SmoothingHeightPainter(TerrainPainter::Dependencies&& dependencies, std::unique_ptr<IBrush> brush)
    : HeightPainter(std::move(dependencies), std::move(brush))
{
}

SmoothingHeightPainter::~SmoothingHeightPainter()
{
}
void SmoothingHeightPainter::Apply(Texture& texture, const vec2ui& paintedPoint, const Influance& influancePoint,
                                   DeltaTime deltaTime)
{
    try
    {
        auto& heightmap = dynamic_cast<HeightMap&>(texture);
        if (not IsInRange(heightmap.GetImage(), paintedPoint))
        {
            LOG_WARN << "Out of range";
            return;
        }
        uint32_t offset = SMOOTH_KERNEL_SIZE / 2;

        const auto& heightMapImage = heightmap.GetImage();
        uint32_t xStart            = (paintedPoint.x > offset) ? paintedPoint.x - offset : 0;
        uint32_t xEnd              = std::min(paintedPoint.x + offset, heightMapImage.width - 1);

        uint32_t yStart = (paintedPoint.y > offset) ? paintedPoint.y - offset : 0;
        uint32_t yEnd   = std::min(paintedPoint.y + offset, heightMapImage.height - 1);

        float sum = 0.0f;
        int count = 0;

        for (auto y = yStart; y <= yEnd; y++)
        {
            for (auto x = xStart; x <= xEnd; x++)
            {
                vec2ui pixel{x, y};

                auto currentHeightOpt = heightmap.GetHeight(pixel);
                if (currentHeightOpt)
                {
                    sum += currentHeightOpt.value();
                    ++count;
                }
            }
        }

        if (count > 0)
        {
            avaragesHeights.push_back(
                Point{.point = paintedPoint, .avarage = sum / static_cast<float>(count), .heightmap = heightmap});
        }
    }
    catch (const std::bad_cast& e)
    {
        std::cerr << "Bad cast: " << e.what() << std::endl;
    }
}
void SmoothingHeightPainter::UpdateTexture(Components::TerrainRendererComponent& component)
{
    for (const auto& point : avaragesHeights)
    {
        point.heightmap.SetHeight(point.point, point.avarage);
    }
    HeightPainter::UpdateTexture(component);
    avaragesHeights.clear();
}
}  // namespace GameEngine
