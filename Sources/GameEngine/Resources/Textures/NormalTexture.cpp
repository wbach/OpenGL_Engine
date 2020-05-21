#include "NormalTexture.h"

#include <Logger/Log.h>

namespace GameEngine
{
NormalTexture::NormalTexture(GraphicsApi::IGraphicsApi& graphicsApi, Image image)
    : Texture(graphicsApi, vec2ui(image.width, image.height))
    , image_(std::move(image))
{
}

void NormalTexture::GpuLoadingPass()
{
    if (image_.floatData.empty() or graphicsObjectId_)
    {
        ERROR_LOG("There was an error loading the texture. floatData is null or is initialized.");
        return;
    }

    DEBUG_LOG("Create normal texture filename");
    auto graphicsObjectId =
        graphicsApi_.CreateTexture(GraphicsApi::TextureType::FLOAT_TEXTURE_3C, GraphicsApi::TextureFilter::LINEAR,
                                   GraphicsApi::TextureMipmap::NONE, size_, &image_.floatData[0]);

    if (graphicsObjectId)
    {
        graphicsObjectId_ = *graphicsObjectId;
        DEBUG_LOG("Texutre id : " + std::to_string(*graphicsObjectId_) + ", " + file_->GetBaseName() + " is in GPU.");
    }
    else
    {
        ERROR_LOG("Texutre not created. ");
    }

    image_.floatData.clear();
}

Image& NormalTexture::GetImage()
{
    return image_;
}
}  // namespace GameEngine
