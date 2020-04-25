#include "GeneralTexture.h"

#include "Logger/Log.h"
#include <Utils/Utils.h>

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
    DEBUG_LOG("Create texutre filename : " + fullpath);

    if (not data_ or graphicsObjectId_)
    {
        ERROR_LOG("There was an error loading the texture : " + filename + ". data is null or is initialized.");
        return;
    }

    auto graphicsObjectId =
        graphicsApi_.CreateTexture(GraphicsApi::TextureType::U8_RGBA, GraphicsApi::TextureFilter::LINEAR,
                                   GraphicsApi::TextureMipmap::NONE, GraphicsApi::BufferAtachment::NONE, size_, data_);

    if (graphicsObjectId)
    {
        graphicsObjectId_ = *graphicsObjectId;
        DEBUG_LOG("File " + filename + " is in GPU.");
    }
    else
    {
        ERROR_LOG("Texutre not created. Filename : " + fullpath);
    }
}
}  // namespace GameEngine
