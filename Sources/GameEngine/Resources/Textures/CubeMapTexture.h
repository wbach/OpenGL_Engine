#pragma once
#include <array>
#include <Utils/Image/Image.h>
#include "Texture.h"

namespace GameEngine
{
class CubeMapTexture : public Texture
{
public:
    CubeMapTexture(GraphicsApi::IGraphicsApi&, const TextureParameters&, const std::string&, std::array<Utils::Image, 6>);
    void GpuLoadingPass() override;

private:
    std::array<Utils::Image, 6> images_;
    std::string name_;
};
}  // namespace GameEngine
