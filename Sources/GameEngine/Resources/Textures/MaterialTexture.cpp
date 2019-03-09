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
}

void MaterialTexture::GpuLoadingPass()
{
    if (image.data.empty() || isInit)
    {
        Error("There was an error loading the texture : " + filename + ". data is null or is initialized.");
        return;
    }

    Log("Create texutre id : " + std::to_string(id) + ", filneame : " + fullpath);
    id = graphicsApi_.CreateTexture(GraphicsApi::TextureType::U8_RGBA, GraphicsApi::TextureFilter::NEAREST,
                                    GraphicsApi::TextureMipmap::LINEAR, GraphicsApi::BufferAtachment::NONE,
                                    vec2ui(image.width, image.height), &image.data[0]);

    if (id == 0)
    {
        image.data.clear();
        Error("Filename : " + filename + " cannot create texture.");
        return;
    }
    if (!keepData)
    {
        image.data.clear();
    }
    isInit = true;

    Log("File " + filename + " is in GPU.");
}

void MaterialTexture::GpuPostLoadingPass()
{
}
}  // namespace GameEngine
