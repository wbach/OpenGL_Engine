#include "GeneralTexture.h"

#include <Logger/Log.h>
#include <Utils/Utils.h>

namespace GameEngine
{
GeneralTexture::GeneralTexture(GraphicsApi::IGraphicsApi& graphicsApi, const vec2ui& size, void* data)
    : Texture(graphicsApi, size)
    , data_(data)
{
}

void GeneralTexture::GpuLoadingPass()
{
    if (not data_ or graphicsObjectId_)
    {
        ERROR_LOG("There was an error loading the texture. data is null or is initialized.");
        return;
    }

    auto graphicsObjectId =
        graphicsApi_.CreateTexture(GraphicsApi::TextureType::U8_RGBA, GraphicsApi::TextureFilter::LINEAR,
                                   GraphicsApi::TextureMipmap::NONE, size_, data_);

    if (graphicsObjectId)
    {
        graphicsObjectId_ = *graphicsObjectId;
    }
    else
    {
        ERROR_LOG("GeneralTexture not created.");
    }
}
}  // namespace GameEngine
