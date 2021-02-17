#pragma once
#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"
#include "GameEngine/Components/Renderer/Grass/GrassRendererComponent.h"
#include "Painter.h"

namespace GameEngine
{
class PlantPainter : public Painter
{
public:
    using Painter::EntryParamters;
    PlantPainter(const EntryParamters&, Components::GrassRendererComponent&);

    void paintImpl() override;
    void setBrush(const std::string&) override;
    std::string selectedBrush() const override;
    std::vector<std::string> avaiableBrushTypes() const override;
    void generatePositions();

private:
    uint32 getNumberOfInstances();
    void createRandomPositions(const vec3&, const TerrainHeightGetter&, float, uint32);

private:
    Components::GrassRendererComponent& grassComponent_;
    TextureBrushType brushType_;
};
}  // namespace GameEngine
