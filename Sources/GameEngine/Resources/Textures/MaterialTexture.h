#pragma once
#include <stdexcept>
#include "Image.h"
#include "Texture.h"

namespace GameEngine
{
class MaterialTexture : public Texture
{
public:
    MaterialTexture(GraphicsApi::IGraphicsApi&, bool keepData, const File&, Image&);
    void GpuLoadingPass() override;
    void SetImage(Image);
    const Image& GetImage() const;
    void SetPixel(const vec2ui&, const Color&);

private:
    Image image_;
    bool keepData = false;
};
}  // namespace GameEngine
