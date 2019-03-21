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
              const std::string& filepath, ImagePtr image);
    virtual void GpuLoadingPass() override;
    virtual void GpuPostLoadingPass() override;
    ImagePtr GetImage();

private:
    ImagePtr image_;
    bool keepData_ = false;
};
}  // namespace GameEngine
