#pragma once
#include <memory>

#include "GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.h"
#include "GameEngine/DebugTools/Painter/TerrainPoint.h"
#include "Painter.h"
#include "TerrainPointGetter.h"

namespace Utils
{
class Image;
}
namespace Input
{
class InputManager;
}

namespace GameEngine
{
class CameraWrapper;
class Projection;
class IBrush;
class Influance;

namespace Components
{
class ComponentController;
class TerrainRendererComponent;
}  // namespace Components

class TerrainPainter : public Painter
{
public:
    struct Dependencies
    {
        Input::InputManager& inputManager;
        Utils::Thread::IThreadSync& threadSync;
        const CameraWrapper& camera;
        const Projection& projection;
        const vec2ui& windowSize;
        const Components::ComponentController& componentController;
    };

    TerrainPainter(Dependencies&, std::unique_ptr<IBrush>, TerrainTextureType);
    ~TerrainPainter() override;

    virtual void Apply(Texture&, const vec2ui&, const Influance&, DeltaTime) = 0;
    virtual void UpdateTexture(Components::TerrainRendererComponent&);

    void CreateInfulanceForBrushSizeInTextureSpace(const vec3& terrainScale, const vec2ui& textureSize);
    void Paint(const DeltaTime& deltaTime) override;
    std::optional<vec2ui> GetPaintendPoint(const TerrainPoint& terrainPoint, const Influance& point);
    vec2ui ConvertToPixelSpace(const TerrainPoint& terrainPoint);
    bool IsInRange(const Utils::Image& image, const vec2ui& pixel);
    void SetOnPointChange(std::function<void(const std::optional<TerrainPoint>&)>);

protected:
    Input::InputManager& inputManager_;
    TerrainPointGetter pointGetter_;
    std::unique_ptr<IBrush> brush_;
    TerrainTextureType textureType_;

    std::optional<TerrainPoint> lastTerrainPoint;
    std::function<void(const std::optional<TerrainPoint>&)> onPointChange;
};

}  // namespace GameEngine
