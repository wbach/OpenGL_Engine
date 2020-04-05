#include "NormalTexture.h"

#include "GameEngine/Renderers/Objects/Terrain/TerrainNormalMapRenderer.h"
#include "Logger/Log.h"

namespace GameEngine
{
NormalTexture::NormalTexture(GraphicsApi::IGraphicsApi& graphicsApi, bool keepData, const std::string& file,
                             const std::string& filepath, std::unique_ptr<Image> image)
    : Texture(graphicsApi, file, filepath)
    , image_(std::move(image))
    , keepData_(keepData)
{
    size_ = vec2ui(image_->width, image_->height);
}

void NormalTexture::GpuLoadingPass()
{
    if (image_->floatData.empty() || isInGpu_)
    {
        ERROR_LOG("There was an error loading the texture : " + filename + ". floatData is null or is initialized.");
        return;
    }

    DEBUG_LOG("Create texutre id : " + std::to_string(graphicsObjectId_) + ", filneame : " + fullpath);
    auto graphicsObjectId = graphicsApi_.CreateTexture(
        GraphicsApi::TextureType::FLOAT_TEXTURE_3C, GraphicsApi::TextureFilter::LINEAR,
        GraphicsApi::TextureMipmap::NONE, GraphicsApi::BufferAtachment::NONE, size_, &image_->floatData[0]);

    if (graphicsObjectId)
    {
        graphicsObjectId_ = *graphicsObjectId;
        isInGpu_ = true;
        DEBUG_LOG("File " + filename + " is in GPU.");
    }
    else
    {
        image_->floatData.clear();
        ERROR_LOG("Texutre not created. Filename : " + fullpath);
    }

    if (not keepData_)
    {
        image_->floatData.clear();
    }
}

void NormalTexture::GpuPostLoadingPass()
{
}

Image* NormalTexture::GetImage()
{
    return image_.get();
}
}  // namespace GameEngine
