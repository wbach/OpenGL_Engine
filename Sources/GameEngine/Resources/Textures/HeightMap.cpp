#include "HeightMap.h"
#include "GameEngine/Renderers/Objects/Terrain/TerrainNormalMapRenderer.h"
#include "Logger/Log.h"

namespace GameEngine
{
HeightMap::HeightMap(GraphicsApi::IGraphicsApi& graphicsApi, bool keepData, const std::string& file,
                     const std::string& filepath, std::unique_ptr<Image> image)
    : Texture(graphicsApi, file, filepath)
    , image_(std::move(image))
    , keepData_(keepData)
{
    size_ = vec2ui(image_->width, image_->height);
}

void HeightMap::GpuLoadingPass()
{
    if (image_->floatData.empty() || isInit)
    {
        ERROR_LOG("There was an error loading the texture : " + filename + ". floatData is null or is initialized.");
        return;
    }

    DEBUG_LOG("Create texutre id : " + std::to_string(graphicsObjectId_) + ", filneame : " + fullpath);
    graphicsObjectId_ = graphicsApi_.CreateTexture(GraphicsApi::TextureType::FLOAT_TEXTURE_1C,
                                                   GraphicsApi::TextureFilter::LINEAR,
                                    GraphicsApi::TextureMipmap::NONE, GraphicsApi::BufferAtachment::NONE, size_,
                                    &image_->floatData[0]);

    if (graphicsObjectId_ == 0)
    {
        image_->data.clear();
        ERROR_LOG("There was an error loading the texture : " + filename + " cannot create texture.");
        return;
    }

    if (not keepData_)
    {
        image_->floatData.clear();
    }

    isInit = true;
    DEBUG_LOG("File " + filename + " is in GPU.");
}

void HeightMap::GpuPostLoadingPass()
{
}

Image* HeightMap::GetImage()
{
    return image_.get();
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
