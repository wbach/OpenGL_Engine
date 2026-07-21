#include "BindlessTexture.h"

#include <Logger/Log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "GameEngine/Resources/DataStorePolicy.h"
#include "GameEngine/Resources/TextureParameters.h"

namespace GameEngine
{
BindlessTexture::BindlessTexture(GraphicsApi::IGraphicsApi& graphicsApi, Utils::Image&& image, const TextureParameters& paramters,
                               const std::optional<FileHandle>& file)
    : Texture(graphicsApi, paramters, vec2ui(image.width, image.height), file)
    , image_(std::move(image))
    , paramters_(paramters)
{
    if ((not file) and textureParamters_.dataStorePolicy == DataStorePolicy::ToRelease)
    {
        LOG_DEBUG << "File not set in texture, data could be lost, override data store police to keep data.";
        paramters_.dataStorePolicy = DataStorePolicy::Store;
    }
}

BindlessTexture::~BindlessTexture()
{
}

void BindlessTexture::GpuLoadingPass()
{
    std::string debugFileNamePrint{"{genertated}"};
    if (file_)
    {
        debugFileNamePrint = file_->getBaseName();
    }
    if (image_.empty() or graphicsObjectId_)
    {
        LOG_ERROR << "There was an error loading the texture : " << debugFileNamePrint << ". data is null or is initialized.";
        return;
    }

    graphicsObjectId_ = graphicsApi_.CreateTexture(image_, paramters_.filter, paramters_.mimap);

    if (graphicsObjectId_)
    {
        isGpuAtual = true;
    }
    else
    {
        LOG_ERROR << "Texutre not created. " << debugFileNamePrint;
    }

    if (paramters_.dataStorePolicy == DataStorePolicy::ToRelease)
    {
        LOG_DEBUG << "[" << debugFileNamePrint << "] Release data from texture. ";
        image_.clearData();
    }
}
void BindlessTexture::UpdateGpuPass()
{
    if (graphicsObjectId_)
    {
        graphicsApi_.UpdateTexture(*graphicsObjectId_, image_);
        isGpuAtual = true;
    }
}
void BindlessTexture::SetImage(Utils::Image&& image)
{
    size_  = vec2ui(image.width, image.height);
    image_ = std::move(image);
}
const Utils::Image& BindlessTexture::GetImage() const
{
    return image_;
}

Utils::Image& BindlessTexture::GetImage()
{
    return image_;
}

Utils::Image BindlessTexture::MoveImage()
{
    return std::move(image_);
}

void BindlessTexture::SetPixel(const vec2ui& position, const Color& color)
{
    image_.setPixel(position, color);
}
void BindlessTexture::setImageData(Utils::ImageData&& data)
{
    image_.moveData(std::move(data));
}
bool BindlessTexture::IsGpuAtual() const
{
    return isGpuAtual;
}
void BindlessTexture::MarkAsNotActualGpuStatus()
{
    isGpuAtual = false;
}
}  // namespace GameEngine
