#include "CubeMapTexture.h"

#include "Logger/Log.h"

namespace GameEngine
{
CubeMapTexture::CubeMapTexture(GraphicsApi::IGraphicsApi& graphicsApi, const std::string& name,
                               std::array<Image, 6> images, bool keepData)
    : Texture(graphicsApi, name, vec2ui(0, 0))
    , images_(std::move(images))
    , keepData_(keepData)
{
    if (images_.size() != 6)
    {
        DEBUG_LOG("Cube map texture need 6 texutres : " + name);
    }
}

void CubeMapTexture::GpuLoadingPass()
{
    if (graphicsObjectId_ or images_.size() != 6)
    {
        ERROR_LOG("There was an error loading the texture : " + file_->GetBaseName() + ". data is null or is initialized.");
        return;
    }

    std::vector<void*> data;
    data.resize(6);

    for (size_t x = 0; x < 6; x++)
    {
        if (images_[x].data.empty())
        {
            ERROR_LOG("There was an error loading the texture : " + file_->GetBaseName() +
                      ". data is null or is initialized. Wrong image : " + std::to_string(x));
            return;
        }
        data[x] = &images_[x].data[0];
    }

    auto graphicsObjectId = graphicsApi_.CreateCubMapTexture(vec2ui(images_[0].width, images_[0].height), data);

    if (graphicsObjectId)
    {
        graphicsObjectId_ = *graphicsObjectId;
        DEBUG_LOG("File " + file_->GetBaseName() + " is in GPU.");
    }
    else
    {
        ERROR_LOG("Texutre not created. Filename : " + file_->GetBaseName());
    }

    for (auto& i : images_)
        i.data.clear();
}
}  // namespace GameEngine
