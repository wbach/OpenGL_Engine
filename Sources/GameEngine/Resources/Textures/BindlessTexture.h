#pragma once
#include <Utils/Image/Image.h>

#include "GameEngine/Resources/DataStorePolicy.h"
#include "GameEngine/Resources/FileHandle.h"
#include "GameEngine/Resources/TextureParameters.h"
#include "Texture.h"

namespace GameEngine
{
class ENGINE_API BindlessTexture : public Texture
{
public:
    BindlessTexture(GraphicsApi::IGraphicsApi&, Utils::Image&&, const TextureParameters&,
                   const std::optional<FileHandle>& = std::nullopt);
    ~BindlessTexture();
    void GpuLoadingPass() override;
    void UpdateGpuPass() override;
    void SetImage(Utils::Image&&);
    void setImageData(Utils::ImageData&&);
    const Utils::Image& GetImage() const;
    Utils::Image& GetImage();
    void SetPixel(const vec2ui&, const Color&);
    Utils::Image MoveImage();

    void MarkAsNotActualGpuStatus();
    bool IsGpuAtual() const;

private:
    Utils::Image image_;
    TextureParameters paramters_;
    bool isGpuAtual{false};
};
}  // namespace GameEngine
