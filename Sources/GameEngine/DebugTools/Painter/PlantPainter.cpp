#include "PlantPainter.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include <functional>
#include <magic_enum/magic_enum.hpp>
#include <random>

#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"
#include "GameEngine/Components/Renderer/Grass/GrassRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/DebugTools/Painter/Painter.h"
#include "GameEngine/DebugTools/Painter/TexturePainter.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "IBrush.h"
#include "Image/Image.h"
#include "TerrainPoint.h"
#include "TerrainPointGetter.h"
#include "Types.h"

namespace GameEngine
{
namespace
{
constexpr float PLANT_STEP = 0.1f;

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

bool IsWorldPosInImageColorRange(const PaintTextureBasedContext& context, const vec3& worldPos)
{
    const auto& blendMap = context.blendmapImageData.get();

    vec3 localPos = (worldPos - (context.terrainPosition - context.terrainScale * 0.5f)) / context.terrainScale;
    localPos.x    = std::clamp(localPos.x, 0.0f, 1.0f);
    localPos.z    = std::clamp(localPos.z, 0.0f, 1.0f);

    auto pixelX = static_cast<uint32>(localPos.x * (blendMap.size().x - 1));
    auto pixelY = static_cast<uint32>(localPos.z * (blendMap.size().y - 1));

    auto color = blendMap.getPixel(vec2ui(pixelX, pixelY));
    if (not color)
    {
        LOG_WARN << "No color in blend map at pixel (" << pixelX << ", " << pixelY << ")";
        return false;
    }

    constexpr float threshold = 0.5f;

    if (context.paintedColor.value.x < threshold && context.paintedColor.value.y < threshold &&
        context.paintedColor.value.z < threshold && context.paintedColor.value.w < threshold)
    {
        const bool isBackground =
            color->value.x < threshold && color->value.y < threshold && color->value.z < threshold && color->value.w < threshold;

        return isBackground;
    }

    float colorValue = 0.0f;
    if (context.paintedColor.value.x > threshold)
        colorValue = color->value.x;
    else if (context.paintedColor.value.y > threshold)
        colorValue = color->value.y;
    else if (context.paintedColor.value.z > threshold)
        colorValue = color->value.z;
    else if (context.paintedColor.value.w > threshold)
        colorValue = color->value.w;
    else
        return false;

    return colorValue > threshold;
}

// bool IntersectRayWithBoundingBox(const Ray& ray, const BoundingBox& box, float& tMin, float& tMax)
// {
//     tMin = 0.0f;
//     tMax = std::numeric_limits<float>::max();

//     const glm::vec3& boxMin = box.min();
//     const glm::vec3& boxMax = box.max();

//     for (int i = 0; i < 3; ++i)
//     {
//         float invD = 1.0f / ray.direction[i];
//         float t0   = (boxMin[i] - ray.origin[i]) * invD;
//         float t1   = (boxMax[i] - ray.origin[i]) * invD;

//         if (invD < 0.0f)
//             std::swap(t0, t1);

//         tMin = std::max(tMin, t0);
//         tMax = std::min(tMax, t1);

//         if (tMax <= tMin)
//             return false;
//     }

//     return true;
// }
// bool IntersectRayWithBoundingBox(const Ray& ray, const BoundingBox& box)
// {
//     float tMin, tMax;
//     return IntersectRayWithBoundingBox(ray, box, tMin, tMax);
// }
// std::optional<glm::vec3> RayHitPoint(const Ray& ray, const BoundingBox& box)
// {
//     float tMin, tMax;
//     if (IntersectRayWithBoundingBox(ray, box, tMin, tMax))
//         return ray.origin + ray.direction * tMin;
//     return std::nullopt;
// }
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
bool RemovePointsWithinRadius(std::vector<float>& positions, const glm::vec3& center, float radius)
{
    const size_t originalSize = positions.size();
    if (originalSize % 3 != 0)
    {
        LOG_WARN << "Positions size is not a multiple of 3.";
        return false;
    }

    const float radiusSquared = radius * radius;
    size_t writeIndex         = 0;

    for (size_t i = 0; i < originalSize; i += 3)
    {
        glm::vec3 p(positions[i], positions[i + 1], positions[i + 2]);
        const float dist2 = glm::distance2(p, center);

        if (dist2 > radiusSquared)
        {
            positions[writeIndex]     = positions[i];
            positions[writeIndex + 1] = positions[i + 1];
            positions[writeIndex + 2] = positions[i + 2];
            writeIndex += 3;
        }
    }

    if (writeIndex < originalSize)
        positions.resize(writeIndex);

    return writeIndex != originalSize;
}
float EstimateBrushRadiusFromInfluence(const std::vector<Influance>& points)
{
    if (points.empty())
        return 0.f;

    float maxDist2 = 0.f;
    for (const auto& p : points)
    {
        glm::vec2 pos = glm::vec2(p.point);
        float dist2   = glm::dot(pos, pos);
        if (dist2 > maxDist2)
            maxDist2 = dist2;
    }

    return std::sqrt(maxDist2);
}
}  // namespace

PlantPainter::PlantPainter(Dependencies&& dependencies, std::unique_ptr<IBrush> brush)
    : Painter(dependencies.threadSync)
    , dependencies(std::move(dependencies))
    , mode(PaintMode::Erase)
    , brush(std::move(brush))
{
    this->brush->createInfluance(false, PLANT_STEP, PLANT_STEP);
}

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
    {
        if (lastMouseState)
        {
            if (onPaintEndCallback_)
            {
                onPaintEndCallback_();
            }
            lastMouseState = false;
        }
        return;
    }

    lastMouseState = lmouseKeyIsPressed;

    if (mode == PaintMode::Terrain)
    {
        Paint(mousePosition);
    }
    else if (mode == PaintMode::Erase)
    {
        Erase(mousePosition);
    }
}

std::vector<PaintTextureBasedContext> PlantPainter::getGenerateContextForTerrainsWithTexture(const File& terrainTextureFile)
{
    std::vector<PaintTextureBasedContext> contexts;
    auto terrains = dependencies.componentController.GetAllActiveComponentsOfType<Components::TerrainRendererComponent>();
    for (const auto& terrainComponent : terrains)
    {
        auto textures = terrainComponent->GetTextures();

        auto blendmap = terrainComponent->GetTexture(TerrainTextureType::blendMap);
        if (not blendmap)
        {
            LOG_WARN << "Terrain has no blendmap!";
            continue;
        }
        auto gtBlendmap = dynamic_cast<GameEngine::GeneralTexture*>(blendmap);
        if (not gtBlendmap)
        {
            LOG_WARN << "Blendmap is not GeneralTexture!";
            continue;
        }

        for (const auto& [type, texture] : textures)
        {
            if (GameEngine::isPaintAbleTexture(type))
            {
                if (texture->GetFile() == terrainTextureFile)
                {
                    auto maybeColor = convertPaintAbleTextureTypeToColor(type);
                    if (not maybeColor)
                    {
                        LOG_WARN << "Texture type is not paintable: " << magic_enum::enum_name(type);
                        continue;
                    }

                    contexts.push_back(PaintTextureBasedContext{
                        .terrainComponent  = terrainComponent,
                        .blendmapImageData = gtBlendmap->GetImage(),
                        .paintedColor      = *maybeColor,
                        .terrainPosition   = terrainComponent->GetParentGameObject().GetWorldTransform().GetPosition(),
                        .terrainScale      = terrainComponent->GetParentGameObject().GetWorldTransform().GetScale()});

                    LOG_DEBUG << "Add PaintTextureBasedContext for texture: " << terrainTextureFile
                              << " in terrain: " << terrainComponent->GetParentGameObject().GetName()
                              << " Color texture in blend map: " << maybeColor;
                }
            }
        }
    }

    return contexts;
}

void PlantPainter::Generate(const File& terrainTextureFile)
{
    LOG_DEBUG << "Generate";

    if (not terrainTextureFile)
    {
        LOG_WARN << "No valid terrain texture file provided for generation.";
        return;
    }

    std::vector<PaintTextureBasedContext> contexts = getGenerateContextForTerrainsWithTexture(terrainTextureFile);

    LOG_DEBUG << "Found contexts size: " << contexts.size();

    for (const auto& context : contexts)
    {
        if (not context.terrainComponent)
        {
            LOG_WARN << "No valid terrain component in context";
            continue;
        }
        if (not context.terrainComponent->GetHeightMap())
        {
            LOG_WARN << "No valid heightmap in terrain component";
            continue;
        }

        TerrainHeightGetter heightGetter(context.terrainScale, *context.terrainComponent->GetHeightMap(),
                                         context.terrainPosition);
        Components::GrassRendererComponent::GrassMeshData pointMeshData;

        auto plantComponent = getPaintedPlantComponent(context.terrainComponent->GetParentGameObject());
        if (not plantComponent)
            continue;

        plantComponent->GetGrassMeshesData() = {};  // TO DO remove only for specyfic texture

        const float step{2.f / density};
        const auto halfScale = context.terrainScale / 2.f;
        for (float z = context.terrainPosition.z - halfScale.x; z < context.terrainPosition.z + halfScale.z; z += step)
        {
            for (float x = context.terrainPosition.x - halfScale.x; x < context.terrainPosition.x + halfScale.x; x += step)
            {
                auto worldpos = randomVec2(randomness, density) + vec2(x, z);

                if (not IsWorldPosInImageColorRange(context, vec3(worldpos.x, 0.f, worldpos.y)))
                {
                    LOG_DEBUG << "World pos not in image color range: " << vec3(worldpos.x, 0.f, worldpos.y);
                    continue;
                }

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
}

void PlantPainter::Generate(const std::optional<IdType>& maybeGameObjectId)
{
    LOG_DEBUG << "Generate terrain specyfic: " << maybeGameObjectId;
    if (not maybeGameObjectId)
    {
        auto terrains = dependencies.componentController.GetAllActiveComponentsOfType<Components::TerrainRendererComponent>();
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
void PlantPainter::Erase(const vec2& mousePosition)
{
    TerrainPointGetter pointGetter(dependencies.camera, dependencies.componentController);
    auto currentTerrainPoint = pointGetter.GetMousePointOnTerrain(mousePosition);
    if (not currentTerrainPoint)
    {
        LOG_WARN << "No terrain point under mouse for erasing plants.";
        return;
    }

    auto& parent         = currentTerrainPoint->terrainComponent->GetParentGameObject();
    auto plantComponents = parent.GetComponents<Components::GrassRendererComponent>();
    if (plantComponents.empty())
    {
        LOG_WARN << "No plant components to erase from.";
        return;
    }

    const auto& points = brush->getInfluence();
    if (points.empty())
    {
        LOG_WARN << "Influance points empty";
        return;
    }

    std::unordered_set<Components::GrassRendererComponent*> modifiedComponents;

    const float radius = EstimateBrushRadiusFromInfluence(points) * PLANT_STEP;

    for (const auto& point : points)
    {
        const auto convertedPoint = vec3(static_cast<float>(point.point.x), 0.f, static_cast<float>(point.point.y)) * PLANT_STEP;
        const glm::vec3 worldSpacePoint = convertedPoint + currentTerrainPoint->pointOnTerrain;

        for (auto* plantComponent : plantComponents)
        {
            auto& meshData = plantComponent->GetGrassMeshesData();
            if (RemovePointsWithinRadius(meshData.positions, worldSpacePoint, radius))
                modifiedComponents.insert(plantComponent);
        }
    }

    for (auto* plantComponent : modifiedComponents)
        plantComponent->UpdateModel();
}

void PlantPainter::Paint(const vec2& mousePosition)
{
    switch (mode)
    {
        case PaintMode::Terrain:
        {
            TerrainPointGetter pointGetter(dependencies.camera, dependencies.componentController);
            auto currentTerrainPoint = pointGetter.GetMousePointOnTerrain(mousePosition);
            if (not currentTerrainPoint)
            {
                return;
            }

            // dla terenow i rysowania mozemy nie jawnie dodawac nowy komponent
            // Jesli teren nie ma zadnego komponentu to towrzymy nowy dla tej tekstury.
            auto plantComponent = getPaintedPlantComponent(currentTerrainPoint->terrainComponent->GetParentGameObject());

            if (not plantComponent)
                return;

            const auto& points = brush->getInfluence();
            if (points.empty())
            {
                LOG_WARN << "Influance points empty";
                return;
            }

            if (onPaintCallback_)
            {
                onPaintCallback_(*plantComponent);
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
            catch (const std::runtime_error& err)
            {
                LOG_DEBUG << err.what();
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
void PlantPainter::SetOnPaintCallback(PaintCallbackType callback)
{
    onPaintCallback_ = std::move(callback);
}
void PlantPainter::SetOnPaintEndCallback(PaintEndCallbackType callback)
{
    onPaintEndCallback_ = std::move(callback);
}
void PlantPainter::SetOnGenerateCallback(GenereteCallbackType callback)
{
    onGenerateCallback_ = std::move(callback);
}
}  // namespace GameEngine
