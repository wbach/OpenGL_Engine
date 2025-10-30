#include "PlantPainter.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include <magic_enum/magic_enum.hpp>

#include "GameEngine/DebugTools/Common/MouseUtils.h"
#include "GameEngine/DebugTools/Painter/Painter.h"
#include "GameEngine/Renderers/StaticRayTracer/Ray.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
#include "IBrush.h"
#include "TerrainPoint.h"
#include "TerrainPointGetter.h"

namespace GameEngine
{
namespace
{
struct Ray
{
    vec3 origin;
    vec3 direction;
};

bool IntersectRayWithBoundingBox(const Ray& ray, const BoundingBox& box, float& tMin, float& tMax)
{
    tMin = 0.0f;
    tMax = std::numeric_limits<float>::max();

    const glm::vec3& boxMin = box.min();
    const glm::vec3& boxMax = box.max();

    for (int i = 0; i < 3; ++i)
    {
        float invD = 1.0f / ray.direction[i];
        float t0   = (boxMin[i] - ray.origin[i]) * invD;
        float t1   = (boxMax[i] - ray.origin[i]) * invD;

        if (invD < 0.0f)
            std::swap(t0, t1);

        tMin = std::max(tMin, t0);
        tMax = std::min(tMax, t1);

        if (tMax <= tMin)
            return false;
    }

    return true;
}
bool IntersectRayWithBoundingBox(const Ray& ray, const BoundingBox& box)
{
    float tMin, tMax;
    return IntersectRayWithBoundingBox(ray, box, tMin, tMax);
}
std::optional<glm::vec3> RayHitPoint(const Ray& ray, const BoundingBox& box)
{
    float tMin, tMax;
    if (IntersectRayWithBoundingBox(ray, box, tMin, tMax))
        return ray.origin + ray.direction * tMin;
    return std::nullopt;
}
}  // namespace
PlantPainter::PlantPainter(Dependencies&& dependencies, std::unique_ptr<IBrush> brush, PaintMode mode, float density,
                           float randomness)
    : Painter(dependencies.threadSync)
    , dependencies(std::move(dependencies))
    , mode(mode)
    , brush(std::move(brush))
    , density(density)
    , randomness(randomness)
{
}
PlantPainter::~PlantPainter()
{
}
void PlantPainter::Paint(const DeltaTime&)
{
    auto mousePosition      = dependencies.inputManager.GetMousePosition();
    auto lmouseKeyIsPressed = dependencies.inputManager.GetMouseKey(KeyCodes::LMOUSE);

    if (not lmouseKeyIsPressed)
        return;

    switch (mode)
    {
        case PaintMode::Terrain:
        {
            TerrainPointGetter pointGetter(dependencies.camera, dependencies.projection, dependencies.componentController);
            auto currentTerrainPoint = pointGetter.GetMousePointOnTerrain(mousePosition);
            if (currentTerrainPoint)
            {
                LOG_DEBUG << currentTerrainPoint;
            }
        }
        break;
        case PaintMode::Mesh:
            // auto rayDir        = CalculateMouseRayDirection(dependencies.projection, dependencies.camera, mousePosition);
        default:
            LOG_WARN << "Not implented : " << magic_enum::enum_name(mode);
    }
}

}  // namespace GameEngine
