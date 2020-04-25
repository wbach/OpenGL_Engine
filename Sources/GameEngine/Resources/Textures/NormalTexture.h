#pragma once
#include <stdexcept>
#include "Image.h"
#include "Texture.h"

namespace GameEngine
{
class NormalTexture : public Texture
{
public:
    NormalTexture(GraphicsApi::IGraphicsApi& graphicsApi, bool keepData, const std::string& file,
              const std::string& filepath, std::unique_ptr<Image> image);
    void GpuLoadingPass() override;
    Image* GetImage();

private:
    std::unique_ptr<Image> image_;
    bool keepData_ = false;
};
}  // namespace GameEngine
