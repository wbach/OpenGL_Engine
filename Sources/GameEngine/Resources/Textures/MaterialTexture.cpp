#include "MaterialTexture.h"

#include "Logger/Log.h"

namespace GameEngine
{
MaterialTexture::MaterialTexture(GraphicsApi::IGraphicsApi& graphicsApi, bool keepData, const std::string& file,
                                 const std::string& filepath, Image& image)
    : Texture(graphicsApi, file, filepath)
    , image(std::move(image))
    , keepData(keepData)
{
    size_ = vec2ui(image.width, image.height);
}

void MaterialTexture::GpuLoadingPass()
{
    if (image.data.empty() || isInit)
    {
        ERROR_LOG("There was an error loading the texture : " + filename + ". data is null or is initialized.");
        return;
    }

    DEBUG_LOG("Create texutre filneame : " + fullpath);

    auto graphicsObjectId = graphicsApi_.CreateTexture(
        GraphicsApi::TextureType::U8_RGBA, GraphicsApi::TextureFilter::NEAREST, GraphicsApi::TextureMipmap::LINEAR,
        GraphicsApi::BufferAtachment::NONE, size_, &image.data[0]);

    if (graphicsObjectId)
    {
        graphicsObjectId_ = *graphicsObjectId;
        isInit            = true;
        DEBUG_LOG("File " + filename + " is in GPU.");
    }
    else
    {
        image.data.clear();
        ERROR_LOG("Texutre not created. Filename : " + fullpath);
    }

    if (!keepData)
    {
        image.data.clear();
    }
}

void MaterialTexture::GpuPostLoadingPass()
{
}
}  // namespace GameEngine
