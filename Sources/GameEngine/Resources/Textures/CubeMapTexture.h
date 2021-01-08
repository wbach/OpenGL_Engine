#pragma once
#include <array>
#include <GraphicsApi/Image.h>
#include "Texture.h"

namespace GameEngine
{
class CubeMapTexture : public Texture
{
public:
    CubeMapTexture(GraphicsApi::IGraphicsApi&, const TextureParameters&, const std::string&, std::array<GraphicsApi::Image, 6>);
    void GpuLoadingPass() override;

private:
    std::array<GraphicsApi::Image, 6> images_;
    std::string name_;
};
}  // namespace GameEngine
