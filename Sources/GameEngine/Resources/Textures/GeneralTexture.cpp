#include "GeneralTexture.h"

#include <Logger/Log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

namespace GameEngine
{
GeneralTexture::GeneralTexture(GraphicsApi::IGraphicsApi& graphicsApi, Utils::Image&& image, const TextureParameters& paramters,
                               const std::optional<File>& file)
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
        LOG_ERROR << "There was an error loading the texture : " << debugFileNamePrint << ". data is null or is initialized.";
        return;
    }

    LOG_DEBUG << "Create texutre : " << debugFileNamePrint;

    auto graphicsObjectId = graphicsApi_.CreateTexture(image_, paramters_.filter, paramters_.mimap);

    if (graphicsObjectId)
    {
        graphicsObjectId_ = *graphicsObjectId;
        LOG_DEBUG << "Texture " << debugFileNamePrint << " is in GPU. GraphicsObjectId :" << *graphicsObjectId;
    }
    else
    {
        image_.clearData();
        LOG_ERROR << "Texutre not created. " << debugFileNamePrint;
    }

    if (paramters_.dataStorePolicy == DataStorePolicy::ToRelease)
    {
        image_.clearData();
    }
}
void GeneralTexture::SetImage(Utils::Image&& image)
{
    size_  = vec2ui(image.width, image.height);
    image_ = std::move(image);
}
const Utils::Image& GeneralTexture::GetImage() const
{
    return image_;
}

Utils::Image& GeneralTexture::GetImage()
{
    return image_;
}

Utils::Image GeneralTexture::MoveImage()
{
    return std::move(image_);
}

void GeneralTexture::SetPixel(const vec2ui& position, const Color& color)
{
    image_.setPixel(position, color);
}
void GeneralTexture::setImageData(Utils::ImageData&& data)
{
    image_.moveData(std::move(data));
}
}  // namespace GameEngine
