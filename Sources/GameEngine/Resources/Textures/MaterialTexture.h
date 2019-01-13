#pragma once
#include <stdexcept>
#include "Image.h"
#include "Texture.h"

namespace GameEngine
{
class MaterialTexture : public Texture
{
public:
    MaterialTexture(IGraphicsApi& graphicsApi, bool keepData, const std::string& file, const std::string& filepath,
        Image& image);
    virtual void GpuLoadingPass() override;
    virtual void GpuPostLoadingPass() override;

private:
    Image image;
    bool keepData = false;
};
}  // namespace GameEngine
