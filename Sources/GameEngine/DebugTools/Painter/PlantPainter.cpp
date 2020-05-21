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
    , numberOfInstances_(strength_ < 0.f ? 1 : static_cast<uint32>(strength_))
{
}
void PlantPainter::SetNumberOfInstances(uint32 v)
{
    numberOfInstances_ = v;
}
void PlantPainter::Paint(const TerrainPoint& point)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    auto range = static_cast<float>(brushSize_ / 2);
    std::uniform_real_distribution<float> dist(-range, static_cast<float>(range));

    TerrainHeightGetter terrainHeightGetter(
        point.terrainComponent.GetTerrainConfiguration(), *point.terrainComponent.GetHeightMap(),
        point.terrainComponent.GetParentGameObject().GetWorldTransform().GetPosition());

    bool positionAdded{false};
    numberOfInstances_ = strength_ < 0.f ? 1 : static_cast<uint32>(strength_);

    for (uint32 i = 0; i < numberOfInstances_; ++i)
    {
        auto newPoint    = point.pointOnTerrain + vec3(dist(mt), 0, dist(mt));
        auto position    = terrainHeightGetter.GetPointOnTerrain(newPoint.x, newPoint.z);
        auto maybeNormal = terrainHeightGetter.GetNormalOfTerrain(newPoint.x, newPoint.z);
        vec3 normal      = maybeNormal ? *maybeNormal : vec3(0, 1, 0);
       // DEBUG_LOG(std::to_string(normal));
        if (position)
        {
            vec2 sizeAndRotation(1.f, 0.f);
            Color color(255, 255, 255);
            Components::GrassRendererComponent::GrassMeshData grassMesh{*position, sizeAndRotation, normal, color};
            grassComponent_.AddGrassMesh(grassMesh);
            positionAdded = true;
        }
    }
    if (positionAdded)
        grassComponent_.UpdateModel();
}
void PlantPainter::SetBrush(const std::string&)
{
}
std::string PlantPainter::SelectedBrush() const
{
    return "CircleBrush";
}
std::vector<std::string> PlantPainter::AvaiableBrushTypes() const
{
    return {"CircleBrush"};
}
}  // namespace GameEngine