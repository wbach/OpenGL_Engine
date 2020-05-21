#include "MaterialTexture.h"
#include <Utils/FileSystem/FileSystemUtils.hpp>
#include "Logger/Log.h"

namespace GameEngine
{
MaterialTexture::MaterialTexture(GraphicsApi::IGraphicsApi& graphicsApi, bool keepData, const File& filename,
                                 Image& image)
    : Texture(graphicsApi, filename, vec2ui(image.width, image.height))
    , image_(std::move(image))
    , keepData(keepData)
{
}

void MaterialTexture::GpuLoadingPass()
{
    if (image_.data.empty() or graphicsObjectId_)
    {
        ERROR_LOG("There was an error loading the texture : " + file_->GetBaseName() + ". data is null or is initialized.");
        return;
    }

    DEBUG_LOG("Create texutre filneame : " + file_->GetBaseName());

    auto graphicsObjectId =
        graphicsApi_.CreateTexture(GraphicsApi::TextureType::U8_RGBA, GraphicsApi::TextureFilter::NEAREST,
                                   GraphicsApi::TextureMipmap::LINEAR, size_, &image_.data[0]);

    if (graphicsObjectId)
    {
        graphicsObjectId_ = *graphicsObjectId;
        DEBUG_LOG("File " + file_->GetBaseName() + " is in GPU.");
    }
    else
    {
        image_.data.clear();
        ERROR_LOG("Texutre not created. Filename : " + file_->GetBaseName());
    }

    if (not keepData)
    {
        image_.data.clear();
    }
}

void MaterialTexture::SetImage(Image image)
{
    image_ = std::move(image);
    size_  = vec2ui(image.width, image.height);
}

const Image& MaterialTexture::GetImage() const
{
    return image_;
}
void MaterialTexture::SetPixel(const vec2ui& position, const Color& color)
{
    if (image_.SetPixel(position, color))
        orginalData_ = false;
}
}  // namespace GameEngine
