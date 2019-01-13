#pragma once
#include <stdexcept>
#include "Image.h"
#include "Texture.h"

namespace GameEngine
{
class HeightMap : public Texture
{
public:
    HeightMap(IGraphicsApi& graphicsApi, bool keepData, const std::string& file,
              const std::string& filepath, ImagePtr image);
    virtual void GpuLoadingPass() override;
    virtual void GpuPostLoadingPass() override;
    ImagePtr GetImage();

private:
    ImagePtr image;
    bool keepData = false;
};
}  // namespace GameEngine
