#include "GeneralTexture.h"
#include "Logger/Log.h"

namespace GameEngine
{
GeneralTexture::GeneralTexture(GraphicsApi::IGraphicsApi& graphicsApi, const std::string& file, const vec2ui& size,
                               void* data)
    : Texture(graphicsApi, file, file)
    , size_(size)
    , data_(data)
{
}

void GeneralTexture::GpuLoadingPass()
{
    if (not data_ || isInit)
    {
        Log("[Error] OGL There was an error loading the texture : " + filename + ". data is null or is initialized.");
        return;
    }

    Log("Create texutre id : " + std::to_string(id) + ", filneame : " + fullpath);
    id = graphicsApi_.CreateTexture(GraphicsApi::TextureType::U8_RGBA, GraphicsApi::TextureFilter::NEAREST,
                                    GraphicsApi::TextureMipmap::NONE, GraphicsApi::BufferAtachment::NONE, size_,
                                    data_);

    isInit = true;

    Log("File " + filename + " is in GPU. OpenGL pass succes");
}

void GeneralTexture::GpuPostLoadingPass()
{
}
}  // namespace GameEngine
