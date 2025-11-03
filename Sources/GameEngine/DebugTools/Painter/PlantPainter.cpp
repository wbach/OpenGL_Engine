#include "PlantPainter.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include <magic_enum/magic_enum.hpp>
#include <random>

#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"
#include "GameEngine/Components/Renderer/Grass/GrassRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/DebugTools/Common/MouseUtils.h"
#include "GameEngine/DebugTools/Painter/Painter.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/StaticRayTracer/Ray.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
#include "IBrush.h"
#include "TerrainPoint.h"
#include "TerrainPointGetter.h"
#include "Types.h"

namespace GameEngine
{
namespace
{
constexpr char PLANTS_GAMEOBJECT_NAME[] = "Plants";

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
PlantPainter::PlantPainter(Dependencies&& dependencies, const File& plantTexture, std::unique_ptr<IBrush> brush, PaintMode mode,
                           float density, float randomness)
    : Painter(dependencies.threadSync)
    , plantTexture(plantTexture)
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
                // dla terenow i rysowania mozemy nie jawnie dodawac nowy komponent

                // Jesli teren nie ma zadnego komponentu to towrzymy nowy dla tej tekstury.
                auto& parent = currentTerrainPoint->terrainComponent->GetParentGameObject();
                Components::GrassRendererComponent* plantComponent{nullptr};
                auto plantComponents = parent.GetComponents<Components::GrassRendererComponent>();
                if (plantComponents.empty())
                {
                    auto& component = parent.AddComponent<Components::GrassRendererComponent>();
                    component.setTexture(plantTexture);
                    plantComponent = &component;
                }
                else
                {
                    // Jesli teren ma jakies  komponenty to szuakmy czy ma jakis dla takiej teksutry, jesli ma to go uzywamy,
                    // jesli nie towrzymy nowy komponent
                    auto iter = std::find_if(plantComponents.begin(), plantComponents.end(),
                                             [this](auto* plantPtr) { return plantPtr->getTextureFile() == plantTexture; });

                    if (iter != plantComponents.end())
                    {
                        plantComponent = *iter;
                    }
                    else
                    {
                        auto& component = parent.AddComponent<Components::GrassRendererComponent>();
                        component.setTexture(plantTexture);
                        plantComponent = &component;
                    }
                }

                const auto& points = brush->getInfluence();
                // LOG_DEBUG << currentTerrainPoint << " points size : " << points.size();
                if (points.empty())
                {
                    LOG_WARN << "Influance points empty";
                }

                Components::GrassRendererComponent::GrassMeshData pointMeshData;

                try
                {
                    auto randomVec2 = [](float randomness, float density) -> glm::vec2
                    {
                        static std::mt19937 rng(std::random_device{}());
                        std::uniform_real_distribution<float> dist(-randomness, randomness);

                        float x = dist(rng) * density;
                        float y = dist(rng) * density;

                        return glm::vec2(x, y);
                    };

                    TerrainHeightGetter heightGetter(*currentTerrainPoint->terrainComponent);
                    bool modelChanged{false};
                    for (const auto& point : points)
                    {
                        auto randomOffset    = randomVec2(randomness, density);
                        auto worldSpacePoint = vec3(static_cast<float>(point.point.x) / density + randomOffset.x, 0.f,
                                                    static_cast<float>(point.point.y) * density + randomOffset.y) +
                                               currentTerrainPoint->pointOnTerrain;

                        auto maybeHeight = heightGetter.GetHeightofTerrain(worldSpacePoint.x, worldSpacePoint.z);
                        auto maybeNormal = heightGetter.GetNormalOfTerrain(worldSpacePoint.x, worldSpacePoint.z);
                        // LOG_DEBUG << "maybeHeight: " << maybeHeight;
                        // LOG_DEBUG << "maybeNormal: " << maybeNormal;
                        if (maybeHeight and maybeNormal)
                        {
                            pointMeshData.position = vec3(worldSpacePoint.x, *maybeHeight, worldSpacePoint.z);
                            LOG_DEBUG << "pointMeshData.position: " << pointMeshData.position;
                            pointMeshData.normal          = *maybeNormal;
                            pointMeshData.color           = Color(vec4(1.f));
                            pointMeshData.sizeAndRotation = vec2(1.f, 0.f);
                            plantComponent->AddGrassMesh(pointMeshData);
                            modelChanged = true;
                        }
                    }

                    if (modelChanged)
                    {
                        plantComponent->UpdateModel();
                    }
                }
                catch (std::runtime_error err)
                {
                    LOG_DEBUG << err.what();
                }
            }
        }
        break;
        // case PaintMode::Mesh:
        //  zwiazku z tym ze nie chcemy aby kazdy mesz obrywal rysowaniem, oczekujemy jawnego posiadania komponentu przy
        //  malowaniu.

        // auto rayDir        = CalculateMouseRayDirection(dependencies.projection, dependencies.camera, mousePosition);
        default:
            LOG_WARN << "Not implented : " << magic_enum::enum_name(mode);
    }
}

}  // namespace GameEngine
