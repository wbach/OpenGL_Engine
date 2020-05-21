#pragma once
#include <array>
#include "Image.h"
#include "Texture.h"

namespace GameEngine
{
class CubeMapTexture : public Texture
{
public:
    CubeMapTexture(GraphicsApi::IGraphicsApi&, const std::string& name, std::array<Image, 6>, bool keepData);
    void GpuLoadingPass() override;

private:
    std::array<Image, 6> images_;
    bool keepData_;
};
}  // namespace GameEngine
