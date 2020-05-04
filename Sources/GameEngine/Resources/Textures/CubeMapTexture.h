#pragma once
#include <array>
#include "Image.h"
#include "Texture.h"

namespace GameEngine
{
class CubeMapTexture : public Texture
{
public:
    CubeMapTexture(GraphicsApi::IGraphicsApi& graphicsApi, const std::string& name, std::array<Image, 6> images, bool keepData);
    void GpuLoadingPass() override;

private:
    std::array<Image, 6> images_;
    bool keepData_ = false;
};
}  // namespace GameEngine
