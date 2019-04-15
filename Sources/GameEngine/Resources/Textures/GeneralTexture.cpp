#include "GeneralTexture.h"
#include "Logger/Log.h"

namespace GameEngine
{
GeneralTexture::GeneralTexture(GraphicsApi::IGraphicsApi& graphicsApi, const std::string& file, const vec2ui& size,
                               void* data)
    : Texture(graphicsApi, file, file)
    , data_(data)
    , size_(size)
{
}

void GeneralTexture::GpuLoadingPass()
{
    if (not data_ || isInit)
    {
        ERROR_LOG("There was an error loading the texture : " + filename + ". data is null or is initialized.");
        return;
    }

    DEBUG_LOG("Create texutre id : " + std::to_string(id) + ", filneame : " + fullpath);
    id = graphicsApi_.CreateTexture(GraphicsApi::TextureType::U8_RGBA, GraphicsApi::TextureFilter::NEAREST,
                                    GraphicsApi::TextureMipmap::NONE, GraphicsApi::BufferAtachment::NONE, size_, data_);

    isInit = true;

    DEBUG_LOG("File " + filename + " is in GPU.");
}

void GeneralTexture::GpuPostLoadingPass()
{
}
}  // namespace GameEngine
