#include "NormalTexture.h"
#include "GameEngine/Renderers/Objects/Terrain/TerrainNormalMapRenderer.h"
#include "Logger/Log.h"

namespace GameEngine
{
    NormalTexture::NormalTexture(GraphicsApi::IGraphicsApi& graphicsApi, bool keepData, const std::string& file,
        const std::string& filepath, ImagePtr image)
        : Texture(graphicsApi, file, filepath)
        , image_(std::move(image))
        , keepData_(keepData)
    {
        size_ = vec2ui(image_->width, image_->height);
    }

    void NormalTexture::GpuLoadingPass()
    {
        if (image_->floatData.empty() || isInit)
        {
            Log("[Error] There was an error loading the texture : " + filename + ". floatData is null or is initialized.");
            return;
        }

        Log("Create texutre id : " + std::to_string(id) + ", filneame : " + fullpath);
        id = graphicsApi_.CreateTexture(GraphicsApi::TextureType::FLOAT_TEXTURE_3C, GraphicsApi::TextureFilter::LINEAR,
            GraphicsApi::TextureMipmap::NONE, GraphicsApi::BufferAtachment::NONE, size_,
            &image_->floatData[0]);

        if (id == 0)
        {
            image_->data.clear();
            Log("[Error] There was an error loading the texture : " + filename + " cannot create texture.");
            return;
        }

        if (not keepData_)
        {
            image_->floatData.clear();
        }

        isInit = true;
        Log("File " + filename + " is in GPU.");
    }

    void NormalTexture::GpuPostLoadingPass()
    {
    }

    ImagePtr NormalTexture::GetImage()
    {
        return image_;
    }
}  // namespace GameEngine
