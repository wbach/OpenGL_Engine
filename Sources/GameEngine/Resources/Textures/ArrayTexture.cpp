#include "ArrayTexture.h"

#include <Logger/Log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <cstddef>

#include "GameEngine/Resources/DataStorePolicy.h"
#include "GameEngine/Resources/TextureParameters.h"
#include "Types.h"

namespace GameEngine
{
namespace
{
vec2ui getTextureSize(const std::vector<Utils::Image>& images)
{
    if (images.empty())
    {
        return vec2ui(0);
    }

    return vec2ui{images.front().width, images.front().height};
}
}  // namespace
ArrayTexture::ArrayTexture(GraphicsApi::IGraphicsApi& graphicsApi, std::vector<Utils::Image>&& images,
                           const TextureParameters& paramters, const std::optional<File>& file)
    : Texture(graphicsApi, paramters, getTextureSize(images), file)
    , images_(std::move(images))
    , paramters_(paramters)
{
    if (file->empty() and textureParamters_.dataStorePolicy == DataStorePolicy::ToRelease)
    {
        LOG_DEBUG << "File not set in texture, data could be lost, override data store police to keep data.";
        paramters_.dataStorePolicy = DataStorePolicy::Store;
    }
}

ArrayTexture::~ArrayTexture()
{
}

void ArrayTexture::GpuLoadingPass()
{
    std::string debugFileNamePrint{"{genertated}"};
    if (file_)
    {
        debugFileNamePrint = file_->GetBaseName();
    }
    if (images_.empty())
    {
        LOG_WARN << "No images in data";
        return;
    }

    LOG_DEBUG << "Create texutre : " << debugFileNamePrint << ", Images vector size " << images_.size();

    auto graphicsObjectId = graphicsApi_.CreateTexture(images_, paramters_.filter, paramters_.mimap);

    if (graphicsObjectId)
    {
        graphicsObjectId_ = *graphicsObjectId;
        LOG_DEBUG << "Texture array" << debugFileNamePrint << " is in GPU. GraphicsObjectId :" << *graphicsObjectId;
    }
    else
    {
        LOG_ERROR << "Texutre not created. " << debugFileNamePrint;
    }

    if (paramters_.dataStorePolicy == DataStorePolicy::ToRelease)
    {
        LOG_DEBUG << "[" << debugFileNamePrint << "] Release data from textures. ";
        for (auto& image : images_)
        {
            image.clearData();
        }
    }
}
void ArrayTexture::SetImages(std::vector<Utils::Image>&& in)
{
    if (in.empty())
    {
        size_ = vec2ui(0);
        images_.clear();
        return;
    }

    size_   = vec2ui(in.front().width, in.front().height);
    images_ = std::move(in);
}
const std::vector<Utils::Image>& ArrayTexture::GetImages() const
{
    return images_;
}

std::vector<Utils::Image>& ArrayTexture::GetImages()
{
    return images_;
}

std::vector<Utils::Image> ArrayTexture::MoveImages()
{
    return std::move(images_);
}

void ArrayTexture::SetPixel(const vec2ui& position, const Color& color, size_t index)
{
    if (index >= images_.size())
    {
        LOG_WARN << "Wrong index texture.";
        return;
    }

    images_[index].setPixel(position, color);
}
void ArrayTexture::setImageData(Utils::ImageData&& data, size_t index)
{
    if (index >= images_.size())
    {
        LOG_WARN << "Wrong index texture.";
        return;
    }

    images_[index].moveData(std::move(data));
}
TextureParameters& ArrayTexture::getTextureParameters()
{
    return paramters_;
}
void ArrayTexture::UpdateDataFromGpu()
{
    if (graphicsObjectId_)
    {
        images_ = graphicsApi_.GetImageArray(*graphicsObjectId_);
    }
}
}  // namespace GameEngine
