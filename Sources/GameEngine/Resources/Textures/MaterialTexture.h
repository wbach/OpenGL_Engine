#pragma once
#include <stdexcept>
#include "Image.h"
#include "Texture.h"

namespace GameEngine
{
class MaterialTexture : public Texture
{
public:
    MaterialTexture(IGraphicsApiPtr graphicsApi, bool keepData, const std::string& file, const std::string& filepath,
                    ImagePtr image);
    virtual void GpuLoadingPass() override;
    virtual void GpuPostLoadingPass() override;

private:
    ImagePtr image;
    bool keepData = false;
};
}  // namespace GameEngine
