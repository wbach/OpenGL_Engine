#pragma once
#include "Painter.h"
#include "GameEngine/DebugTools/Painter/Brushes/Circle/CircleTextureBrushes/CircleLinearTextureBrush.h"

namespace GameEngine
{
class TerrainTexturePainter : public Painter
{
public:
    using Painter::EntryParamters;

    TerrainTexturePainter(const EntryParamters&, const Color&);
    void paintImpl() override;
    void setColor(const Color&);
    void setBrush(const std::string&) override;
    std::string selectedBrush() const override;
    std::vector<std::string> avaiableBrushTypes() const override;
    float getWorldScaleBrushSize() override;

private:
    void createBrush();
    void calculateWorldScaleBrushSize() override;

private:
    Color color_;
    TextureBrushType brushType_;
    float worldScaleBrushSize_;
    std::unique_ptr<CircleLinearTextureBrush> brush_;
};
}  // namespace GameEngine
