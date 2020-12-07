#include "PlantPainter.h"

#include <random>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

#include <Logger/Log.h>

namespace GameEngine
{
PlantPainter::PlantPainter(const EntryParamters& entryParamters, Components::GrassRendererComponent& component)
    : Painter(entryParamters, PaintType::Plant, 30.f, 16)
    , grassComponent_(component)
    , numberOfInstances_(0)
{
}
void PlantPainter::setNumberOfInstances(uint32 v)
{
    numberOfInstances_ = v;
}
void PlantPainter::paintImpl()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    auto range = static_cast<float>(paintContext_.brushSize / 2);
    std::uniform_real_distribution<float> dist(-range, static_cast<float>(range));

    std::uniform_real_distribution<float> ySizeDist(0.75, 1.25);
    std::uniform_real_distribution<float> rotationDist(0.f, 360.f);
    std::uniform_int_distribution<int> colorGreenDist(200, 255);
    std::uniform_int_distribution<int> colorBlueDist(200, 255);

    const auto& point = *paintContext_.currentTerrainPoint;

    TerrainHeightGetter terrainHeightGetter(
        point.terrainComponent.GetTerrainConfiguration(), *point.terrainComponent.GetHeightMap(),
        point.terrainComponent.GetParentGameObject().GetWorldTransform().GetPosition());

    bool positionAdded{false};
    updateNumberOfInstances();

    for (uint32 i = 0; i < numberOfInstances_; ++i)
    {
        auto newPoint    = point.pointOnTerrain + vec3(dist(mt), 0, dist(mt));
        auto position    = terrainHeightGetter.GetPointOnTerrain(newPoint.x, newPoint.z);
        auto maybeNormal = terrainHeightGetter.GetNormalOfTerrain(newPoint.x, newPoint.z);
        vec3 normal      = maybeNormal ? *maybeNormal : vec3(0, 1, 0);
       // DEBUG_LOG(std::to_string(normal));
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
void PlantPainter::updateNumberOfInstances()
{
    numberOfInstances_ = paintContext_.strength < 0.f ? 1 : static_cast<uint32>(paintContext_.strength);
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
}  // namespace GameEngine
