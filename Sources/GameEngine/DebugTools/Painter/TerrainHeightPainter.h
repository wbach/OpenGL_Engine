#pragma once
#include "Painter.h"

namespace GameEngine
{
class TerrainHeightPainter : public Painter
{
public:
    using Painter::EntryParamters;
    TerrainHeightPainter(const EntryParamters&);

    void Paint(const TerrainPoint&) override;
    void SetBrush(const std::string&) override;
    void RecalculateTerrainNormals();
    std::string SelectedBrush() const override;
    std::vector<std::string> AvaiableBrushTypes() const override;

private:
    HeightBrushType heightBrushType_;
};
}  // namespace GameEngine
