#pragma once
#include <Utils/Image/Image.h>

#include "GameEngine/Resources/DataStorePolicy.h"
#include "GameEngine/Resources/TextureParameters.h"
#include "Texture.h"

namespace GameEngine
{
class GeneralTexture : public Texture
{
public:
    GeneralTexture(GraphicsApi::IGraphicsApi&, Utils::Image&&, const TextureParameters&,
                   const std::optional<File>& = std::nullopt);
    ~GeneralTexture();
    void GpuLoadingPass() override;
    void SetImage(Utils::Image&&);
    void setImageData(Utils::ImageData&&);
    const Utils::Image& GetImage() const;
    Utils::Image& GetImage();
    void SetPixel(const vec2ui&, const Color&);
    Utils::Image MoveImage();

private:
    Utils::Image image_;
    TextureParameters paramters_;
};
}  // namespace GameEngine
