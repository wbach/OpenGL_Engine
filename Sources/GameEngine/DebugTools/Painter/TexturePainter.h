#pragma once
#include <optional>

#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/ITextureLoader.h"
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
    ITextureLoader& textureLoader;
    File requestedFileTexture;
    std::optional<Color> paintedColor;
};
}  // namespace GameEngine
