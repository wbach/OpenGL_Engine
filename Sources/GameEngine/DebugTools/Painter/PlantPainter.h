#pragma once
#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"
#include "GameEngine/Components/Renderer/Grass/GrassComponent.h"
#include "Painter.h"

namespace GameEngine
{
class PlantPainter : public Painter
{
public:
    using Painter::EntryParamters;
    PlantPainter(const EntryParamters&, Components::GrassRendererComponent&);

    void setNumberOfInstances(uint32);
    void paintImpl() override;
    void setBrush(const std::string&) override;
    std::string selectedBrush() const override;
    std::vector<std::string> avaiableBrushTypes() const override;

private:
    void updateNumberOfInstances();

private:
    Components::GrassRendererComponent& grassComponent_;
    TextureBrushType brushType_;
    uint32 numberOfInstances_;
};
}  // namespace GameEngine
