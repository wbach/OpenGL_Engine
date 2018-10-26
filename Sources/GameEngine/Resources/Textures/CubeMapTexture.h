#pragma once
#include <stdexcept>
#include <vector>
#include "Image.h"
#include "Texture.h"

namespace GameEngine
{
class CubeMapTexture : public Texture
{
public:
    CubeMapTexture(IGraphicsApiPtr graphicsApi, const std::string& name,
                    const std::vector<ImagePtr>& image);
    virtual void GpuLoadingPass() override;
    virtual void GpuPostLoadingPass() override;

private:
    ImagePtr images[6];
    bool keepData = false;
};
}  // namespace GameEngine
