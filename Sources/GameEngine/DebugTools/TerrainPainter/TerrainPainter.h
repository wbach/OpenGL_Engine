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

class TerrainPainter
{
public:
    TerrainPainter(const CameraWrapper&, const Projection&, const vec2ui&, const Components::ComponentController&);
    void PaintBlendMap(const vec2&, const vec3& color, float range);
    void PaintHeightMap(const vec2&, float height, float range);
    std::optional<vec3> GetMouseTerrainPosition(const vec2&);

private:
    TerrainPointGetter pointGetter_;
};
}  // namespace GameEngine
