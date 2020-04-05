#include "HeightMap.h"
#include "GameEngine/Renderers/Objects/Terrain/TerrainNormalMapRenderer.h"
#include "Logger/Log.h"

namespace GameEngine
{
HeightMap::HeightMap(GraphicsApi::IGraphicsApi& graphicsApi, bool keepData, const std::string& file,
                     const std::string& filepath, Image image)
    : Texture(graphicsApi, file, filepath)
    , image_(std::move(image))
    , keepData_(keepData)
{
    size_ = vec2ui(image_.width, image_.height);
}

void HeightMap::GpuLoadingPass()
{
    if (image_.floatData.empty() or isInGpu_)
    {
        ERROR_LOG("There was an error loading the texture : " + filename + ". floatData is null or is initialized.");
        return;
    }

    DEBUG_LOG("Create texutre filneame : " + fullpath);
    auto graphicsObjectId = graphicsApi_.CreateTexture(GraphicsApi::TextureType::FLOAT_TEXTURE_1C,
                                                   GraphicsApi::TextureFilter::LINEAR,
                                    GraphicsApi::TextureMipmap::NONE, GraphicsApi::BufferAtachment::NONE, size_,
                                    &image_.floatData[0]);

    if (graphicsObjectId)
    {
        graphicsObjectId_ = *graphicsObjectId;
        isInGpu_ = true;
        DEBUG_LOG("File " + filename + " is in GPU.");
    }
    else
    {
        image_.floatData.clear();
        ERROR_LOG("Texutre not created. Filename : " + fullpath);
    }

    if (not keepData_)
    {
        image_.floatData.clear();
    }
}

void HeightMap::GpuPostLoadingPass()
{
}

Image* HeightMap::GetImage()
{
    return &image_;
}

void HeightMap::SetScale(const vec3& scale)
{
    scale_ = scale;
}

const vec3& HeightMap::GetScale() const
{
    return scale_;
}
}  // namespace GameEngine
