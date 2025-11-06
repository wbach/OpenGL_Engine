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
struct Ray
{
    vec3 origin;
    vec3 direction;
};

vec2 randomVec2(float randomness, float density)
{
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(-randomness, randomness);

    float x = dist(rng) * density;
    float y = dist(rng) * density;

    return vec2(x, y);
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
// Globalny / thread_local RNG — seed raz, lepiej niż rand()
static thread_local std::mt19937 rng{std::random_device{}()};
static thread_local std::uniform_real_distribution<float> uni01(0.0f, 1.0f);

// Helper zwracający los w [-1, 1]
inline float randSigned()
{
    return uni01(rng) * 2.0f - 1.0f;
}

// Funkcja aplikująca randomness
void ApplyColorAndSizeRandomness(Components::GrassRendererComponent::GrassMeshData& pointMeshData, const Color& baseColor,
                                 const vec3& colorRnd,  // colorRandomness.r/g/b (0..1)
                                 float sizeRandomness)  // (0..1) oznacza +- %
{
    // 1) Kolor - mnożymy proporcjonalnie do wartości bazowej:
    float rFactor = randSigned() * colorRnd[0];  // w [-colorRnd[0], +colorRnd[0]]
    float gFactor = randSigned() * colorRnd[1];
    float bFactor = randSigned() * colorRnd[2];

    float newR = baseColor.value.x * (1.0f + rFactor);
    float newG = baseColor.value.y * (1.0f + gFactor);
    float newB = baseColor.value.z * (1.0f + bFactor);

    // clamp do [0,1]
    newR = std::clamp(newR, 0.0f, 1.0f);
    newG = std::clamp(newG, 0.0f, 1.0f);
    newB = std::clamp(newB, 0.0f, 1.0f);

    // zachowaj alfa (w niektórych strukturach alpha może być w .w)
    pointMeshData.color.value.x = newR;
    pointMeshData.color.value.y = newG;
    pointMeshData.color.value.z = newB;
    // jeżeli masz alpha w baseColor.w:
    pointMeshData.color.value.w = baseColor.value.w;

    // 2) Wielkość: chcesz scale = 1 ± sizeRandomness
    float sizeFactor = randSigned() * sizeRandomness;  // [-sizeRandomness, +sizeRandomness]
    float newScale   = 1.0f + sizeFactor;
    // minimalny dopuszczalny scale, żeby nie dostać < 0
    const float MIN_SCALE = 0.0001f;
    if (newScale < MIN_SCALE)
        newScale = MIN_SCALE;

    // Przykład: sizeAndRotation.x = scale, .y = rotation (0 tutaj)
    pointMeshData.sizeAndRotation.x = newScale;
    pointMeshData.sizeAndRotation.y = 0.0f;
}

}  // namespace
PlantPainter::PlantPainter(Dependencies&& dependencies, const File& plantTexture, std::unique_ptr<IBrush> brush, PaintMode mode,
                           const Color& baseColor, const vec3& colorRandomness, float sizeRandomness, float density,
                           float randomness)
    : Painter(dependencies.threadSync)
    , plantTexture(plantTexture)
    , dependencies(std::move(dependencies))
    , mode(mode)
    , brush(std::move(brush))
    , baseColor(baseColor)
    , colorRandomness(colorRandomness)
    , sizeRandomness(sizeRandomness)
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
                auto plantComponent = getPaintedPlantComponent(currentTerrainPoint->terrainComponent->GetParentGameObject());

                const auto& points = brush->getInfluence();
                // LOG_DEBUG << currentTerrainPoint << " points size : " << points.size();
                if (points.empty())
                {
                    LOG_WARN << "Influance points empty";
                }

                Components::GrassRendererComponent::GrassMeshData pointMeshData;

                try
                {
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
                            pointMeshData.normal = *maybeNormal;
                            pointMeshData.color  = baseColor;
                            ApplyColorAndSizeRandomness(pointMeshData, baseColor, colorRandomness, sizeRandomness);
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

void PlantPainter::Generate(const File& terrainTextureFile)
{
    LOG_DEBUG << "not implmented yet";

    if (not terrainTextureFile)
    {
        // TerrainSelectionDialog dialog(nullptr, dependencies.componentController, "Whole terrain mode selected. Select terrain
        // to generate plants"); auto selection = dialog.GetSelection();
    }
}
void PlantPainter::Generate(const std::optional<IdType>& maybeGameObjectId)
{
    LOG_DEBUG << "Generate terrain specyfic: " << maybeGameObjectId;
    if (not maybeGameObjectId)
    {
        auto terrains = dependencies.componentController.GetAllComponentsOfType<Components::TerrainRendererComponent>();
        for (const auto& terrainComponent : terrains)
        {
            GenerateOnTerrain(terrainComponent);
        }
    }
    else
    {
        if (auto gameObject = dependencies.scene.GetGameObject(*maybeGameObjectId))
        {
            auto terrainComponent = gameObject->GetComponent<Components::TerrainRendererComponent>();
            if (terrainComponent)
            {
                GenerateOnTerrain(terrainComponent);
            }
            else
            {
                LOG_WARN << "GameObject id: " << *maybeGameObjectId << " has no TerrainRendererComponent";
            }
        }
        else
        {
            LOG_WARN << "No GameObject with id: " << *maybeGameObjectId;
        }
    }
}
Components::GrassRendererComponent* PlantPainter::getPaintedPlantComponent(GameObject& parent)
{
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
    return plantComponent;
}

void PlantPainter::GenerateOnTerrain(Components::TerrainRendererComponent* terrainComponent)
{
    auto& gameObject      = terrainComponent->GetParentGameObject();
    const auto& transform = gameObject.GetWorldTransform();
    const auto pos        = transform.GetPosition();
    const auto scale      = transform.GetScale();

    TerrainHeightGetter heightGetter(scale, *terrainComponent->GetHeightMap(), pos);
    Components::GrassRendererComponent::GrassMeshData pointMeshData;

    auto plantComponent = getPaintedPlantComponent(gameObject);
    if (not plantComponent)
        return;

    plantComponent->GetGrassMeshesData() = {};

    const float step{2.f / density};
    const auto halfScale = scale / 2.f;
    for (float z = pos.z - halfScale.x; z < pos.z + halfScale.z; z += step)
    {
        for (float x = pos.x - halfScale.x; x < pos.x + halfScale.x; x += step)
        {
            auto worldpos    = randomVec2(randomness, density) + vec2(x, z);
            auto maybeHeight = heightGetter.GetHeightofTerrain(worldpos.x, worldpos.y);
            auto maybeNormal = heightGetter.GetNormalOfTerrain(worldpos.x, worldpos.y);

            if (maybeHeight and maybeNormal)
            {
                pointMeshData.position = vec3(worldpos.x, *maybeHeight, worldpos.y);
                pointMeshData.normal   = *maybeNormal;
                pointMeshData.color    = baseColor;
                ApplyColorAndSizeRandomness(pointMeshData, baseColor, colorRandomness, sizeRandomness);
                plantComponent->AddGrassMesh(pointMeshData);
            }
        }
    }

    plantComponent->UpdateModel();
}
}  // namespace GameEngine
