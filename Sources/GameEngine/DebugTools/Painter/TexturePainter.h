#pragma once
#include <memory>
#include <optional>
#include <unordered_map>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/ITextureLoader.h"
#include "Image/Image.h"
#include "TerrainPainter.h"

namespace GameEngine
{
class TexturePainter : public TerrainPainter
{
public:
    using TerrainPainter::Dependencies;
    TexturePainter(TerrainPainter::Dependencies&&, ITextureLoader&, std::unique_ptr<IBrush>, const File&);

    bool PreparePaint(TerrainPoint&) override;
    void Apply(Texture&, const vec2ui&, const Influance&, DeltaTime) override;
    void UpdateTexture(Components::TerrainRendererComponent&) override;

private:
    void CreateBlendMapIfNeeded(Components::TerrainRendererComponent&);
    Utils::Image* GetPaintableImage(Components::TerrainRendererComponent&);
    std::optional<Color> GetPaintedColor(Components::TerrainRendererComponent&);

private:
    ITextureLoader& textureLoader;
    File requestedFileTexture;

    struct PaintedContext
    {
        Color paintedColor;
        std::reference_wrapper<Utils::Image> paintedImage;
    };

    std::optional<PaintedContext> currentPaintingContext;

    std::unordered_map<Components::TerrainRendererComponent*, std::optional<PaintedContext>> paintedComponents;
    std::unordered_map<Components::TerrainRendererComponent*, std::unique_ptr<Utils::Image>> tmpfloatingImages;
};
}  // namespace GameEngine
