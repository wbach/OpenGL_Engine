#include "TerrainPainter.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include <magic_enum/magic_enum.hpp>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "IBrush.h"
#include "TerrainPoint.h"

namespace GameEngine
{
TerrainPainter::TerrainPainter(Dependencies& dependencies, std::unique_ptr<IBrush> brush, TerrainTextureType type)
    : Painter(dependencies.threadSync)
    , inputManager_(dependencies.inputManager)
    , pointGetter_(dependencies.camera, dependencies.projection, dependencies.windowSize, dependencies.componentController)
    , brush_{std::move(brush)}
    , textureType_{type}

{
}
TerrainPainter::~TerrainPainter()
{
    LOG_DEBUG << "";
}
bool TerrainPainter::PreparePaint(TerrainPoint&)
{
    return true;
}
void TerrainPainter::UpdateTexture(Components::TerrainRendererComponent&)
{
}
void TerrainPainter::CreateInfulanceForBrushSizeInTextureSpace(const vec3& terrainScale, const vec2ui& textureSize)
{
    const double stepX = static_cast<double>(terrainScale.x) / static_cast<double>(textureSize.x - 1);
    const double stepZ = static_cast<double>(terrainScale.z) / static_cast<double>(textureSize.y - 1);

    brush_->createInfluance(textureSize.x % 2 == 0, stepX, stepZ);
}
void TerrainPainter::Paint(const DeltaTime& deltaTime)
{
    // GetMousePointOnTerrain rozszeczyc o detekcje terranu poprzez bounding box , na ktory klilamy aby mozna bylo przchodzi
    // dynamicznie pomiedzy terenami.
    auto currentTerrainPoint = pointGetter_.GetMousePointOnTerrain(inputManager_.GetMousePosition());

    if (onPointChange and lastTerrainPoint != currentTerrainPoint)
    {
        lastTerrainPoint = currentTerrainPoint;
        onPointChange(currentTerrainPoint);
    }

    auto lmouseKeyIsPressed = inputManager_.GetMouseKey(KeyCodes::LMOUSE);
    if (currentTerrainPoint and lmouseKeyIsPressed)
    {
        if (not PreparePaint(*currentTerrainPoint))
            return;

        if (not brush_)
        {
            LOG_ERROR << "Brush not set!";
            return;
        }

        const auto& transform = currentTerrainPoint->terrainComponent->getParentGameObject().GetWorldTransform();
        auto* texture         = currentTerrainPoint->terrainComponent->GetTexture(textureType_);
        if (not texture)
        {
            LOG_WARN << "Texture not found. Type = " << magic_enum::enum_name(textureType_);
            return;
        }

        const auto& terrainScale = transform.GetScale();

        CreateInfulanceForBrushSizeInTextureSpace(terrainScale, texture->GetSize());

        const auto& points = brush_->getInfluence();
        if (points.empty())
        {
            LOG_WARN << "Influance points empty";
        }

        for (const auto& point : points)
        {
            if (auto paintedPoint = GetPaintendPoint(*currentTerrainPoint, point))
            {
                Apply(*texture, *paintedPoint, point, deltaTime);
            }
        }

        UpdateTexture(*currentTerrainPoint->terrainComponent);
    }
}
std::optional<vec2ui> TerrainPainter::GetPaintendPoint(const TerrainPoint& terrainPoint, const Influance& point)
{
    vec2i offset  = point.point;
    vec2ui center = ConvertToPixelSpace(terrainPoint);

    if (offset.x < 0 && static_cast<int32>(center.x) < static_cast<int32>(-offset.x))
        return std::nullopt;

    if (offset.y < 0 && static_cast<int32>(center.y) < static_cast<int32>(-offset.y))
        return std::nullopt;

    vec2ui paintedPoint(center.x + offset.x, center.y + offset.y);
    return paintedPoint;
}
vec2ui TerrainPainter::ConvertToPixelSpace(const TerrainPoint& terrainPoint)
{
    const auto& heightmapSize = terrainPoint.terrainComponent->GetTexture(TerrainTextureType::heightmap)->GetSize();

    auto u = terrainPoint.terrainSpacePoint.x;
    auto v = terrainPoint.terrainSpacePoint.y;

    auto px = static_cast<uint32>(std::round(u * (heightmapSize.x - 1)));
    auto py = static_cast<uint32>(std::round(v * (heightmapSize.y - 1)));

    return {px, py};
}
bool TerrainPainter::IsInRange(const Utils::Image& image, const vec2ui& pixel)
{
    const auto& size = image.size();
    return pixel.x < size.x && pixel.y < size.y;
}
void TerrainPainter::SetOnPointChange(std::function<void(const std::optional<TerrainPoint>&)> func)
{
    onPointChange = func;
}
}  // namespace GameEngine