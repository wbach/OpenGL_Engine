#pragma once
#include "Painter.h"
#include "Brushes/HeightBrushType.h"

namespace GameEngine
{
class CircleHeightBrush;

class TerrainHeightPainter : public Painter
{
public:
    using Painter::EntryParamters;
    TerrainHeightPainter(const EntryParamters&);
    ~TerrainHeightPainter();

    void paintImpl() override;
    void setBrush(const std::string&) override;
    void recalculateTerrainNormals();
    std::string selectedBrush() const override;
    std::vector<std::string> avaiableBrushTypes() const override;
    float getWorldScaleBrushSize() override;

private:
    void calculateWorldScaleBrushSize() override;
    void createBrush();
    template <class T>
    void makeBrush();

private:
    HeightBrushType heightBrushType_;
    std::unique_ptr<CircleHeightBrush> brush_;
    float worldScaleBrushSize_;
};
}  // namespace GameEngine
