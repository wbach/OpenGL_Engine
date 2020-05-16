#include "PlantPainter.h"

#include <random>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
PlantPainter::PlantPainter(const EntryParamters& entryParamters, Components::GrassRendererComponent& component)
    : Painter(entryParamters, PaintType::Plant)
    , grassComponent_(component)
    , numberOfInstances_(20)
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
    for (int i = 0; i < numberOfInstances_; ++i)
    {
        auto newPoint = point.pointOnTerrain + vec3(dist(mt), 0, dist(mt));
        auto pos      = terrainHeightGetter.GetPointOnTerrain(newPoint.x, newPoint.z);
        if (pos)
        {
            grassComponent_.AddNextPosition(*pos);
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