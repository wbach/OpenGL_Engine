#include "PlantPainter.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include <functional>
#include <magic_enum/magic_enum.hpp>
#include <random>
#include <unordered_map>
#include <vector>

#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"
#include "GameEngine/Components/Renderer/Grass/GrassRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/DebugTools/Painter/Painter.h"
#include "GameEngine/DebugTools/Painter/TexturePainter.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
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

static thread_local std::mt19937 rng{std::random_device{}()};
static thread_local std::uniform_real_distribution<float> uni01(0.0f, 1.0f);

inline float randSigned()
{
    return uni01(rng) * 2.0f - 1.0f;
}

vec4 GetColorAndSizeRandomness(const Color& baseColor, const vec3& colorRnd, float sizeRandomness)
{
    vec4 result;
    float rFactor = randSigned() * colorRnd[0];
    float gFactor = randSigned() * colorRnd[1];
    float bFactor = randSigned() * colorRnd[2];

    float newR = baseColor.value.x * (1.0f + rFactor);
    float newG = baseColor.value.y * (1.0f + gFactor);
    float newB = baseColor.value.z * (1.0f + bFactor);

    newR = std::clamp(newR, 0.0f, 1.0f);
    newG = std::clamp(newG, 0.0f, 1.0f);
    newB = std::clamp(newB, 0.0f, 1.0f);

    result.x = newR;
    result.y = newG;
    result.z = newB;

    float sizeFactor = randSigned() * sizeRandomness;
    result.w         = 1.0f + sizeFactor;

    const float MIN_SCALE = 0.0001f;
    if (result.w < MIN_SCALE)
        result.w = MIN_SCALE;

    return result;
}
std::vector<vec3> PointsToRemoveWithinRadius(std::vector<Components::GrassRendererComponent::Ssbo>& ssbos,
                                             const glm::vec3& center, float radius)
{
    std::vector<vec3> result;
    const float radiusSquared = radius * radius;

    for (const auto& ssbo : ssbos)
    {
        auto p            = vec3(ssbo.position.value);
        const float dist2 = glm::distance2(p, center);
        if (dist2 > radiusSquared)
        {
            result.push_back(p);
        }
    }
    return result;
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

        auto plantComponent = getPaintedPlantComponent(context.terrainComponent->GetParentGameObject());
        if (not plantComponent)
            continue;

        const float step{2.f / density};
        const auto halfScale = context.terrainScale / 2.f;

        std::vector<Components::GrassRendererComponent::Ssbo> ssbos;

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
                    auto colorAndScaleRandomness = GetColorAndSizeRandomness(baseColor, colorRandomness, sizeRandomness);

                    Components::GrassRendererComponent::Ssbo ssbo{.position = {vec4(worldpos.x, *maybeHeight, worldpos.y, 0.f)},
                                                                  .rotation = {vec4{0}},
                                                                  .normal   = {vec4(*maybeNormal, 0.f)},
                                                                  .colorAndSizeRandomness = {colorAndScaleRandomness}};
                    ssbos.push_back(ssbo);
                }
            }
        }
        plantComponent->UpdateSsbo(std::move(ssbos));
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

    auto plantComponent = getPaintedPlantComponent(gameObject);
    if (not plantComponent)
        return;

    const float step{2.f / density};
    const auto halfScale = scale / 2.f;

    std::vector<Components::GrassRendererComponent::Ssbo> ssbos;
    for (float z = pos.z - halfScale.x; z < pos.z + halfScale.z; z += step)
    {
        for (float x = pos.x - halfScale.x; x < pos.x + halfScale.x; x += step)
        {
            auto worldpos    = randomVec2(randomness, density) + vec2(x, z);
            auto maybeHeight = heightGetter.GetHeightofTerrain(worldpos.x, worldpos.y);
            auto maybeNormal = heightGetter.GetNormalOfTerrain(worldpos.x, worldpos.y);

            if (maybeHeight and maybeNormal)
            {
                auto colorAndScaleRandomness = GetColorAndSizeRandomness(baseColor, colorRandomness, sizeRandomness);

                Components::GrassRendererComponent::Ssbo ssbo{.position = {vec4(worldpos.x, *maybeHeight, worldpos.y, 0.f)},
                                                              .rotation = {vec4{0}},
                                                              .normal   = {vec4(*maybeNormal, 0.f)},
                                                              .colorAndSizeRandomness = {colorAndScaleRandomness}};
                ssbos.push_back(ssbo);
            }
        }
    }

    plantComponent->UpdateSsbo(std::move(ssbos));
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

    const float radius = EstimateBrushRadiusFromInfluence(points) * PLANT_STEP;

    std::unordered_map<Components::GrassRendererComponent*, std::vector<vec3>> modifiedComponents;
    for (const auto& point : points)
    {
        const auto convertedPoint = vec3(static_cast<float>(point.point.x), 0.f, static_cast<float>(point.point.y)) * PLANT_STEP;
        const glm::vec3 worldSpacePoint = convertedPoint + currentTerrainPoint->pointOnTerrain;

        for (auto* plantComponent : plantComponents)
        {
            auto toRemove = PointsToRemoveWithinRadius(plantComponent->GetSsbo()->GetData(), worldSpacePoint, radius);

            auto& p = modifiedComponents[plantComponent];
            p.insert(p.end(), toRemove.begin(), toRemove.end());
        }
    }

    for (auto& [plantComponent, toRemove] : modifiedComponents)
    {
        plantComponent->RemoveInstances(toRemove);
    }
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

            try
            {
                std::vector<Components::GrassRendererComponent::Ssbo> ssbos;
                ssbos.reserve(points.size());

                TerrainHeightGetter heightGetter(*currentTerrainPoint->terrainComponent);
                bool modelHasChanged{false};
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
                        auto colorAndScaleRandomness = GetColorAndSizeRandomness(baseColor, colorRandomness, sizeRandomness);

                        Components::GrassRendererComponent::Ssbo ssbo{
                            .position               = {vec4(worldSpacePoint.x, *maybeHeight, worldSpacePoint.z, 0.f)},
                            .rotation               = {vec4{0}},
                            .normal                 = {vec4(*maybeNormal, 0.f)},
                            .colorAndSizeRandomness = {colorAndScaleRandomness}};
                        ssbos.push_back(ssbo);
                        modelHasChanged = true;
                    }
                }

                if (modelHasChanged)
                {
                    plantComponent->AddInstances(std::move(ssbos));
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
