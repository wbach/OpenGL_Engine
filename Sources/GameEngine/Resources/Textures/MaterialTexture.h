#pragma once
#include <stdexcept>
#include "Image.h"
#include "Texture.h"

namespace GameEngine
{
class MaterialTexture : public Texture
{
public:
    MaterialTexture(GraphicsApi::IGraphicsApi& graphicsApi, bool keepData, const std::string& file,
                    const std::string& filepath, Image& image);
    void GpuLoadingPass() override;
    Image& GetImage();
    const Image& GetImage() const;

private:
    Image image;
    bool keepData = false;
};
}  // namespace GameEngine
