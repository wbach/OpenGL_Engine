#pragma once
#include "Painter.h"

namespace GameEngine
{
class TerrainTexturePainter : public Painter
{
public:
    using Painter::EntryParamters;

    TerrainTexturePainter(const EntryParamters&, const Color&);
    void Paint(const vec2&, const TerrainPoint&) override;
    void SetColor(const Color&);
    void SetBrush(const std::string&) override;
    std::string SelectedBrush() const override;
    std::vector<std::string> AvaiableBrushTypes() const override;

private:
    Color color_;
    TextureBrushType brushType_;
};
}  // namespace GameEngine
