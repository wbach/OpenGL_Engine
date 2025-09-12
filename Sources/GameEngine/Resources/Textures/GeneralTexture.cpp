#include "GeneralTexture.h"

#include <Logger/Log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

namespace GameEngine
{
GeneralTexture::GeneralTexture(GraphicsApi::IGraphicsApi& graphicsApi, Utils::Image image,
                               const TextureParameters& paramters, const std::optional<File>& file)
    : Texture(graphicsApi, paramters, vec2ui(image.width, image.height), file)
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
        /* LOG TO FIX*/  LOG_ERROR << ("There was an error loading the texture : " + debugFileNamePrint +
                  ". data is null or is initialized.");
        return;
    }

    /* LOG TO FIX*/  LOG_ERROR << ("Create texutre : " + debugFileNamePrint);

    auto graphicsObjectId =
        graphicsApi_.CreateTexture(image_, paramters_.filter, paramters_.mimap);

    if (graphicsObjectId)
    {
        graphicsObjectId_ = *graphicsObjectId;
        /* LOG TO FIX*/  LOG_ERROR << ("Texture " + debugFileNamePrint +
                  " is in GPU. GraphicsObjectId :" + std::to_string(*graphicsObjectId));
    }
    else
    {
        image_.clearData();
        /* LOG TO FIX*/  LOG_ERROR << ("Texutre not created. " + debugFileNamePrint);
    }

    if (paramters_.dataStorePolicy == DataStorePolicy::ToRelease)
    {
        image_.clearData();
    }
}
void GeneralTexture::SetImage(Utils::Image image)
{
    image_       = std::move(image);
    size_        = vec2ui(image.width, image.height);
    orginalData_ = false;
}
const Utils::Image& GeneralTexture::GetImage() const
{
    return image_;
}
void GeneralTexture::SetPixel(const vec2ui& position, const Color& color)
{
    if (image_.setPixel(position, color))
        orginalData_ = false;
}
}  // namespace GameEngine
