#include "PlantPainter.h"

#include <Logger/Log.h>

#include <random>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
PlantPainter::PlantPainter(const EntryParamters& entryParamters, Components::GrassRendererComponent& component)
    : Painter(entryParamters, PaintType::Plant, 30.f, 16)
    , grassComponent_(component)
{
}
void PlantPainter::paintImpl()
{
    if (not paintContext_.currentTerrainPoint)
        return;

    const auto& point      = *paintContext_.currentTerrainPoint;
    auto range             = static_cast<float>(paintContext_.brushSize / 2);
    auto numberOfInstances = getNumberOfInstances();

    TerrainHeightGetter terrainHeightGetter(
        point.terrainComponent.getParentGameObject().GetWorldTransform().GetScale(),
        *point.terrainComponent.GetHeightMap(),
        point.terrainComponent.GetParentGameObject().GetWorldTransform().GetPosition());

    createRandomPositions(point.pointOnTerrain, terrainHeightGetter, range, numberOfInstances);
}
uint32 PlantPainter::getNumberOfInstances()
{
    return paintContext_.strength < 0.f ? 1 : static_cast<uint32>(paintContext_.strength);
}
void PlantPainter::createRandomPositions(const vec3& pointOnTerrain, const TerrainHeightGetter& terrainHeightGetter,
                                         float range, uint32 numberOfInstances)
{
    std::random_device rd;
    std::mt19937 mt(rd());

    std::uniform_real_distribution<float> dist(-range, static_cast<float>(range));

    std::uniform_real_distribution<float> ySizeDist(0.75, 1.25);
    std::uniform_real_distribution<float> rotationDist(0.f, 360.f);
    std::uniform_int_distribution<int> colorGreenDist(200, 255);
    std::uniform_int_distribution<int> colorBlueDist(200, 255);

    bool positionAdded{false};

    for (uint32 i = 0; i < numberOfInstances; ++i)
    {
        auto newPoint    = pointOnTerrain + vec3(dist(mt), 0, dist(mt));
        auto position    = terrainHeightGetter.GetPointOnTerrain(newPoint.x, newPoint.z);
        auto maybeNormal = terrainHeightGetter.GetNormalOfTerrain(newPoint.x, newPoint.z);
        vec3 normal      = maybeNormal ? *maybeNormal : vec3(0, 1, 0);

        if (position)
        {
            vec2 sizeAndRotation(ySizeDist(mt), rotationDist(mt));
            Color color(255, colorGreenDist(mt), colorBlueDist(mt));
            Components::GrassRendererComponent::GrassMeshData grassMesh{*position, sizeAndRotation, normal, color};
            grassComponent_.AddGrassMesh(grassMesh);
            positionAdded = true;
        }
    }
    if (positionAdded)
        grassComponent_.UpdateModel();
}
void PlantPainter::setBrush(const std::string&)
{
}
std::string PlantPainter::selectedBrush() const
{
    return "CircleBrush";
}
std::vector<std::string> PlantPainter::avaiableBrushTypes() const
{
    return {"CircleBrush"};
}
void PlantPainter::generatePositions()
{
    DEBUG_LOG("generatePositions");
    auto terrainRendererComponents = componentController_.GetAllComonentsOfType<Components::TerrainRendererComponent>();

    for (auto& [_, terrainComponent] : terrainRendererComponents)
    {
        auto& terrainRendererComponent = *static_cast<Components::TerrainRendererComponent*>(terrainComponent);

        TerrainHeightGetter terrainHeightGetter(
            terrainComponent->getParentGameObject().GetWorldTransform().GetScale(),
            *terrainRendererComponent.GetHeightMap(),
            terrainComponent->GetParentGameObject().GetWorldTransform().GetPosition());

        auto halfScale = terrainComponent->getParentGameObject().GetWorldTransform().GetScale() / 2.f;
        auto position  = terrainComponent->GetParentGameObject().GetWorldTransform().GetPosition();

        vec2 start = position - halfScale;
        vec3 end   = position + halfScale;

        auto range = static_cast<float>(paintContext_.brushSize / 2);
        auto numberOfInstances = getNumberOfInstances();

        for (float y = start.y; y < end.y; y += range)
        {
            for (float x = start.x; x < end.x; x += range)
            {
                auto pointOnTerrain = terrainHeightGetter.GetPointOnTerrain(x, y);
                if (pointOnTerrain)
                {
                    createRandomPositions(*pointOnTerrain, terrainHeightGetter, range, numberOfInstances);
                }
            }
        }
    }
}
}  // namespace GameEngine
