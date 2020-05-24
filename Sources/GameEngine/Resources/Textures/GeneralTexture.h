#pragma once
#include <GraphicsApi/Image.h>

#include "GameEngine/Resources/DataStorePolicy.h"
#include "GameEngine/Resources/TextureParameters.h"
#include "Texture.h"

namespace GameEngine
{
class GeneralTexture : public Texture
{
public:
    GeneralTexture(GraphicsApi::IGraphicsApi&, GraphicsApi::Image, const TextureParameters&,
                   const std::optional<File>& = std::nullopt);
    ~GeneralTexture();
    void GpuLoadingPass() override;
    void SetImage(GraphicsApi::Image);
    const GraphicsApi::Image& GetImage() const;
    void SetPixel(const vec2ui&, const Color&);

private:
    GraphicsApi::Image image_;
    TextureParameters paramters_;
};
}  // namespace GameEngine
