#pragma once
#include <Utils/Image/Image.h>

#include "GameEngine/Resources/DataStorePolicy.h"
#include "GameEngine/Resources/TextureParameters.h"
#include "Texture.h"

namespace GameEngine
{
class ArrayTexture : public Texture
{
public:
    ArrayTexture(GraphicsApi::IGraphicsApi&, std::vector<Utils::Image>&&, const TextureParameters&,
                 const std::optional<File>& = std::nullopt);
    ~ArrayTexture();
    void GpuLoadingPass() override;
    void SetImages(std::vector<Utils::Image>&&);
    void setImageData(Utils::ImageData&&, size_t);
    const std::vector<Utils::Image>& GetImages() const;
    std::vector<Utils::Image>& GetImages();
    void SetPixel(const vec2ui&, const Color&, size_t);
    std::vector<Utils::Image> MoveImages();
    TextureParameters& getTextureParameters();
    void UpdateDataFromGpu();
    void CompressData();
    void DecompressData();

private:
    std::vector<Utils::Image> images_;
    TextureParameters paramters_;
};
}  // namespace GameEngine
