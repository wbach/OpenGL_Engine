#include "HeightMap.h"
#include "Logger/Log.h"

namespace GameEngine
{
HeightMap::HeightMap(GraphicsApi::IGraphicsApi& graphicsApi, bool keepData, const std::string& file,
                     const std::string& filepath, ImagePtr image)
    : Texture(graphicsApi, file, filepath)
    , image(std::move(image))
    , keepData(keepData)
{
}

void HeightMap::GpuLoadingPass()
{
    if (image->floatData.empty() || isInit)
    {
        Log("[Error] OGL There was an error loading the texture : " + filename +
            ". floatData is null or is initialized.");
        return;
    }

    Log("Create texutre id : " + std::to_string(id) + ", filneame : " + fullpath);
    id = graphicsApi_.CreateTexture(GraphicsApi::TextureType::FLOAT_TEXTURE_1C, GraphicsApi::TextureFilter::LINEAR, GraphicsApi::TextureMipmap::NONE,
        GraphicsApi::BufferAtachment::NONE, vec2ui(image->width, image->height), &image->floatData[0]);

    if (id == 0)
    {
        image->data.clear();
        Log("[Error] OGL There was an error loading the texture : " + filename + " cannot create texture.");
        return;
    }

    if (!keepData)
    {
        image->floatData.clear();
    }
    isInit = true;
    Log("File " + filename + " is in GPU. OpenGL pass succes");
}

void HeightMap::GpuPostLoadingPass()
{

}

ImagePtr HeightMap::GetImage()
{
    return image;
}
}  // namespace GameEngine
