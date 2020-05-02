#pragma once
#include <Types.h>

#include <memory>
#include <optional>

#include "TerrainPointGetter.h"

namespace common
{
class Transform;
}  // namespace common

namespace GameEngine
{
class GameObject;
class HeightMap;

namespace Components
{
class ComponentController;
}  // namespace Components

enum class HeightBrushType
{
    CircleLinear,
    CircleAverage
};

class TerrainPainter
{
public:
    TerrainPainter(const CameraWrapper&, const Projection&, const vec2ui&, const Components::ComponentController&);
    void PaintBlendMap(const vec2&, const vec3& color, float range);
    void PaintHeightMap(HeightBrushType type, const vec2&, float height, int32 brushSize);
    std::optional<vec3> GetMouseTerrainPosition(const vec2&);

private:
    TerrainPointGetter pointGetter_;
};
}  // namespace GameEngine
