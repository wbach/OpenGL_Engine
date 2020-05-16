#pragma once
#include "Painter.h"
#include "GameEngine/Components/Renderer/Grass/GrassComponent.h"
#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"

namespace GameEngine
{
class PlantPainter : public Painter
{
public:
    using Painter::EntryParamters;
    PlantPainter(const EntryParamters&, Components::GrassRendererComponent&);
    
    void SetNumberOfInstances(uint32);
    
    void Paint(const TerrainPoint&) override;
    void SetBrush(const std::string&) override;
    std::string SelectedBrush() const override;
    std::vector<std::string> AvaiableBrushTypes() const override;

private:
    Components::GrassRendererComponent& grassComponent_;
    TextureBrushType brushType_;
    uint32 numberOfInstances_;
};
}  // namespace GameEngine
