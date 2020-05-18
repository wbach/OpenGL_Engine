#pragma once
#include <stdexcept>
#include "Image.h"
#include "Texture.h"

namespace GameEngine
{
class NormalTexture : public Texture
{
public:
    NormalTexture(GraphicsApi::IGraphicsApi& graphicsApi, const InputFileName& name, Image image);
    void GpuLoadingPass() override;
    Image& GetImage();

private:
    Image image_;
};
}  // namespace GameEngine
