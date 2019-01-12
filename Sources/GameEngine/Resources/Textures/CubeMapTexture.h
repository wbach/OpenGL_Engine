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
                    std::vector<Image>& image);

    virtual void GpuLoadingPass() override;
    virtual void GpuPostLoadingPass() override;

private:
    std::vector<Image> images_;
    bool keepData_ = false;
};
}  // namespace GameEngine
