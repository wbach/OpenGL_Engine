#include "GeneralTexture.h"

#include <Logger/Log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

namespace GameEngine
{
GeneralTexture::GeneralTexture(GraphicsApi::IGraphicsApi& graphicsApi, GraphicsApi::Image image,
                               const TextureParameters& paramters, const std::optional<File>& file)
    : Texture(graphicsApi, vec2ui(image.width, image.height), file)
    , image_(std::move(image))
    , paramters_(paramters)
{
}

GeneralTexture::~GeneralTexture()
{
}

void GeneralTexture::GpuLoadingPass()
{
    std::string debugFileNamePrint{"{genertated}"};
    if (file_)
    {
        debugFileNamePrint = file_->GetBaseName();
    }
    if (image_.empty() or graphicsObjectId_)
    {
        ERROR_LOG("There was an error loading the texture : " + debugFileNamePrint +
                  ". data is null or is initialized.");
        return;
    }

    DEBUG_LOG("Create texutre : " + debugFileNamePrint);

    auto graphicsObjectId =
        graphicsApi_.CreateTexture(image_, paramters_.filter, paramters_.mimap);

    if (graphicsObjectId)
    {
        graphicsObjectId_ = *graphicsObjectId;
        DEBUG_LOG("Texture " + debugFileNamePrint +
                  " is in GPU. GraphicsObjectId :" + std::to_string(*graphicsObjectId));
    }
    else
    {
        image_.clearData();
        ERROR_LOG("Texutre not created. " + debugFileNamePrint);
    }

    if (paramters_.dataStorePolicy == DataStorePolicy::ToRelease)
    {
        image_.clearData();
    }
}
void GeneralTexture::SetImage(GraphicsApi::Image image)
{
    image_       = std::move(image);
    size_        = vec2ui(image.width, image.height);
    orginalData_ = false;
}
const GraphicsApi::Image& GeneralTexture::GetImage() const
{
    return image_;
}
void GeneralTexture::SetPixel(const vec2ui& position, const Color& color)
{
    if (image_.setPixel(position, color))
        orginalData_ = false;
}
}  // namespace GameEngine
