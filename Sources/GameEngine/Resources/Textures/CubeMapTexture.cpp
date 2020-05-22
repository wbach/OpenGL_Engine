#include "CubeMapTexture.h"

#include "Logger/Log.h"

namespace GameEngine
{
CubeMapTexture::CubeMapTexture(GraphicsApi::IGraphicsApi& graphicsApi, const std::string& name,
                               std::array<GraphicsApi::Image, 6> images)
    : Texture(graphicsApi, vec2ui(0, 0), std::nullopt)
    , images_(std::move(images))
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
        ERROR_LOG("There was an error loading the texture : " + file_->GetBaseName() +
                  ". data is null or is initialized.");
        return;
    }
    auto graphicsObjectId = graphicsApi_.CreateCubMapTexture(images_);

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
        i.clearData();
}
}  // namespace GameEngine
